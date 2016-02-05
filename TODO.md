### TODO
In no particular order. When everything's on fire, which one do you put out first?


* Don't malloc the stuff that doesn't need to be.
* The big nasty: threading/forking to handle simultaneous requests?
* Change the strlist implementation to operate on the head of lists instead of the tail. Gets rid of the ugly list_start junk.
* Add readline(fd, &buf) function?
* Should realloc be used instead in various places?
* Add a form field somewhere and add POST handling
* Make request reading less terrible
