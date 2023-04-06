void execute() {
	// ignore: for NEWLINE
	if(strcmp(wordArray[0], "") == 0) 
		return;

	// track amount of pipes
	int pipeCount = 0;
	// basically pipeCount + 1
	int commandCount = 0;

	// track indices
	int readIndex = 0;
	int writeIndex = 0;
	int appendIndex = 0;
	int standardErrorFileIndex = 0;
	int standardErrorOutputIndex = 0;
	int ampersandIndex = 0; 

	// for the loops
	int i;

	// check if wants to run in background
	if(wordArray[wordCount-1] == "&")
		ampersandIndex = 1;

	// for wildcarding
	int globCount = 0;

	// doing all executions in the child process
	int childpid;
	if((childpid = fork()) == -1) {
		perror("Forking error.");
		printf("Error at line %d\n", __LINE__);
		exit(0);
		return;
	}	

	// in parent
	if(childpid != 0) { 
		// wait for process to finish executing
		if(ampersandIndex == 0) 
			wait((int *) 0);
	}

	// in child
	else {
		// used to track the location of each pipe 
		int* pipes = malloc(300 * sizeof(int));
		// tracks the pipes
		addedWords = 0;
		for(i = 0; i < wordCount; ++i) {
			if(strcmp(wordArray[i], "|") == 0) 
				pipes[pipeCount++] = i;				
		}

		// number of commands is pipeCount + 1
		commandCount = pipeCount + 1;
		
		// resolves aliases in the array
		// aliases at the begining of command
		for(i = 0; i < commandCount; ++i) {
			// possible alias location: first command
			if(i == 0) {
				// infinite alias expansion
				if(strcmp(aliasResolver(wordArray[i]), "INFIN") == 0) { 
					perror("Infinite alias expansion.");
					printf("Error at line %d\n", __LINE__);
					exit(0);
					return;
				}				
				// alias has a value
				else if(strcmp(aliasResolver(wordArray[i]), "") != 0) { 
					char* resolved = strdup(aliasResolver(wordArray[i]));					
					wordArray[i] = resolved;
					wordArrayAliasExpansion(wordArray[i], i + addedWords);
				}
			}
			// possible alias location: after pipe symbol
			else {
				int j;
				// infinite alias expansion
				if(strcmp(aliasResolver(wordArray[pipes[i - 1] + 1]), "INFIN") == 0) {
					perror("Infinite alias expansion.");
					printf("Error at line %d\n", __LINE__);
					exit(0);
					return;
				}
				// alias has a value 
				else if(strcmp(aliasResolver(wordArray[pipes[i - 1] + 1 + addedWords]), "") != 0) {
					char* resolved = strdup(aliasResolver(wordArray[pipes[i - 1] + 1 + addedWords]));					
					wordArray[pipes[i - 1] + 1 + addedWords] = resolved;
					wordArrayAliasExpansion(wordArray[pipes[i - 1] + 1 + addedWords], pipes[i - 1] + 1 + addedWords);
				}
			}
		}

		// track where the globs patterns are
		int* globs = malloc(300 * sizeof(int));
		globCount = 0;
		addedWords = 0;
		for(i = 0; i < wordCount; ++i) {
			// contains a * or ?
			if(strchr(wordArray[i], '*') != NULL || strchr(wordArray[i], '?') != NULL) { 
				globs[globCount++] = i;
			}
		}
		//takes care of globbing
		for(i = 0; i < globCount; ++i) {
			if(i == 0) {
				printf(" ");
			}

			// get all directories to sort through
			char* result = strdup(getDirectories(wordArray[globs[i] + addedWords]));
			if(strcmp(result, "") == 0) { //error
				perror("No matches");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
			cardsGoneWild(result, globs[i] + addedWords);
		}

		
		pipeCount = 0;
		// set indexes
		for(i = 0; i < wordCount; ++i) {
			// redo since alias expansion 
			if(strcmp(wordArray[i], "|") == 0) { //it's a pipe
				pipes[pipeCount++] = i;
			}
			// read
			else if(strcmp(wordArray[i], "<") == 0) { 
				readIndex = i;
			}
			// write 
			else if(strcmp(wordArray[i], ">") == 0) {
				writeIndex = i;
			}
			// append
			else if(strcmp(wordArray[i], ">>") == 0) { 
				appendIndex = i;
			}
			// standard error redirect 2 
			else if(strcmp(wordArray[i], "2>&1") == 0) { 
				standardErrorOutputIndex = i;
			}
			// standard error redirect 1
			else if(strcmp(wordArray[i], "2>") == 0) { 
				standardErrorFileIndex = i;
			}
			// ampersand
			else if(strcmp(wordArray[i], "&") == 0) { 
				ampersandIndex = i;
			}
		}

		// read
		if(readIndex != 0) {
			int in = open(wordArray[readIndex + 1], O_RDONLY, 0); 
			if(in == -1) {
				perror("File not opened");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}

			// redirect input from file
			if (dup2(in, 0) == -1) {
				perror("Input not redirected");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
			close(in);
		}
		// write
		if(writeIndex != 0) {
			int out = open(wordArray[writeIndex + 1], O_WRONLY | O_CREAT, 0755); 
			if(out == -1) {
				perror("File not created");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}

			// redirect output to file
			if (dup2(out, 1) == -1) {
				perror("Output not redirected");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
			close(out);
		}
		// append - add to end of file
		if (appendIndex != 0) {
			int out = open(wordArray[appendIndex + 1], O_WRONLY | O_APPEND | O_CREAT,0755); 
			if(out == -1) { //error
				perror("File not created");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}

			// redirect output to file
			if (dup2(out, 1) == -1) {//error
				perror("Output not redirected");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
			close(out);
		}

		// redirecting error to this
		if(standardErrorOutputIndex != 0) {
			if(dup2(1, 2) == -1) {
				exit(0);
				return;
			}
		}
		if(standardErrorFileIndex != 0) { 
			// get file name after >
			char* copyText = strdup(&wordArray[standardErrorOutputIndex][2]);
						
			int out = open(copyText, O_WRONLY | O_APPEND | O_CREAT, 0755); 
			if(out == -1) {//error
				perror("File not created");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
			//redirect standard error to output file
			if (dup2(out, 2) == -1) {//error
				perror("Standard error not redirected");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
			close(out);
		}

		// reset commandCount
		commandCount = pipeCount + 1;
		int endOfCommand = 0;
		
		// no pipes
		if(pipeCount == 0) { 
			if(readIndex != 0) { 
				endOfCommand = readIndex;
			} else if(writeIndex != 0) { 
				endOfCommand = writeIndex;
			} else if(appendIndex != 0) {
				endOfCommand = appendIndex;
			} else if(standardErrorOutputIndex != 0) { 
				endOfCommand = standardErrorOutputIndex;
			} else if(standardErrorFileIndex != 0) { 
				endOfCommand = standardErrorFileIndex;
			} else if(ampersandIndex != 0) { 
				endOfCommand = ampersandIndex;
			} else { // just normal command with no special needs
				endOfCommand = wordCount;
			}

			char* arguments[endOfCommand + 1];
			int i;
			for(i = 0; i < endOfCommand; i++) {
				arguments[i] = wordArray[i]; 
			}
			arguments[endOfCommand] = (char *)0; 

			// run command	
			if(execvp(arguments[0], arguments) == -1) { 
				perror("Error execvp.");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
		}

		// pod piper
		else {
			if(readIndex != 0) { 
				endOfCommand = readIndex;
			} else if(writeIndex != 0) { 
				endOfCommand = writeIndex;
			} else if(appendIndex != 0) {
				endOfCommand = appendIndex;
			} else if(standardErrorOutputIndex != 0) { 
				endOfCommand = standardErrorOutputIndex;
			} else if(standardErrorFileIndex != 0) { 
				endOfCommand = standardErrorFileIndex;
			} else if(ampersandIndex != 0) { 
				endOfCommand = ampersandIndex;
			} else { // just normal command with no special needs
				endOfCommand = wordCount;
			}

			struct command* commandArray = malloc(commandCount * sizeof(struct command));
			
			// looperino pipeCounterino+1
			for(i = 0; i < commandCount; ++i) {

				// FIRST
				if(i == 0) { 
					char* arguments [pipes[0] + 1];
					int j = 0;
					for(j = 0; j < pipes[0]; ++j) {
						arguments[j] = wordArray[j]; 
					}
					arguments[pipes[0]] = (char*)0; 
					
					commandArray[0].argv = malloc(j*sizeof(char*));
					int k = 0;
					// copy arguments over
					for(k = 0; k < j; ++k) {
						commandArray[0].argv[k] = malloc((strlen(arguments[k]) + 1) * sizeof(char));
						strcpy(commandArray[0].argv[k], arguments[k]);
					}
					commandArray[0].argv[j] = (char*) 0;
				}
				// LAST
				else if(i == (commandCount - 1)) { 
					char* arguments[endOfCommand - pipes[i - 1]];
					int j;
					for(j = 0; j < endOfCommand - pipes[i - 1] - 1; ++j) {
						arguments[j] = wordArray[pipes[i - 1] + 1 + j]; 
					}
					arguments[endOfCommand - pipes[i - 1] - 1] = (char *)0; //null terminator
					commandArray[commandCount - 1].argv = malloc(j*sizeof(char*));
					int k = 0;
					// copy arguments over
					for(k = 0; k < j; ++k) {
						commandArray[commandCount - 1].argv[k] = malloc((strlen(arguments[k]) + 1) * sizeof(char));
						strcpy(commandArray[commandCount - 1].argv[k], arguments[k]);
					}
					commandArray[commandCount - 1].argv[j] = (char*) 0;
				}
				// MID
				else { 
					char* arguments[pipes[i] - pipes[i - 1]];
					int j;
					for(j = 0; j < pipes[i] - pipes[i - 1] - 1; ++j) {
						arguments[j] = wordArray[pipes[i - 1] + 1 + j]; 
					}
					arguments[pipes[i] - pipes[i - 1] - 1] = (char *)0; 
					commandArray[i].argv = malloc(j*sizeof(char*));
					int k = 0;
					for(k = 0; k < j; ++k) {
						commandArray[i].argv[k] = malloc((strlen(arguments[k]) + 1) * sizeof(char));
						strcpy(commandArray[i].argv[k], arguments[k]);
					}
					commandArray[i].argv[j] = (char*) 0;
				}		
			}
			int x;
			int inPipe;
			int outIn [2];

			// create the pipeline
			inPipe = 0;

			/* All but the last part of the pipeline.  */
			for (x = 0; x < commandCount - 1; ++x) {
				pipe(outIn);
				
				pid_t pid;

				// parent
				if ((pid = fork()) == 0) { 
			    	if (inPipe != 0) {
						dup2 (inPipe, 0);
						close (inPipe);
					}
					if (outIn[1] != 1) {
						dup2 (outIn [1], 1);
						close (outIn [1]);
					}
					execvp ((commandArray + x)->argv[0], (char * const *)(commandArray + x)->argv);
			    }

				close (outIn [1]);
				inPipe = outIn [0];
		    }

		    /* Last stage of the pipeline - set stdin be the read end of the previous pipe
		    and output to the original file descriptor 1. */  
		    if (inPipe != 0) {
				dup2(inPipe, 0);
			}
			/* Execute the last stage with the current process. */
			if(execvp (commandArray[x].argv [0], (char * const *)commandArray[x].argv) == -1) { //error
				perror("Error execvp.");
				printf("Error at line %d\n", __LINE__);
				exit(0);
				return;
			}
		}
		exit(0);
	}
}