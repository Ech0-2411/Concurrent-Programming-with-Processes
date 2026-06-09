#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "amap.h"
#include "aux.h"

/* reducer - reads (word,count) pairs from different scanners via pipes,
 *  uses the map to sum them up.
 * When the pipe is at EOF, use amap_getnext() to dump them one by one
 * into the pipe the driver reads from.
 * The pipes handle flow control and indicate when we finish, so we don't
 * need any other synchronization.
 */

void reduce(int numprocs, int me, amap_t *map,
	    pipe_t *reducepipes, pipe_t *driverpipes) {

  /******* YOUR CODE HERE ****/

// Close unnecessary pipe ends
for (int i = 0; i < numprocs; i++) {
  if (i != me)
      close(reducepipes[i].readfd);
  close(reducepipes[i].writefd);
  if (i != me)
      close(driverpipes[i].writefd);
  close(driverpipes[i].readfd);
}

// Read (word, count) pairs from my reduce pipe
char word[MAXSTRING];
int count;
while (readpair(reducepipes[me].readfd, word, &count) > 0) {
  amap_incr(map, word, count);
}

// Send aggregated map to driver
while (amap_getnext(map, word, &count)) {
  writepair(driverpipes[me].writefd, word, &count);
}

close(driverpipes[me].writefd);
exit(0);


  exit(0);
}    
