#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main ()
{
  pid_t childPID;

  childPID = fork ();
  //Check if child. If not, then sleep
  if (childPID != 0) {
    sleep (30);
  }
  //If child, then exit
  else {
    exit (0);
  }
  return 0;
}
