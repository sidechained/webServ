// execute an upload.php script in a child process, passing it a file
// does the child process have to be a copy of our env, or can we build the env from scratch?
// don't forget to include a timeout
// fork, then execve

int main() {
	char *const args[] = {"/usr/bin/php", "upload.php", "output.txt", NULL};
	execve("/usr/bin/php", args, envp);
}