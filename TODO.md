### TODO
In no particular order. When everything's on fire, which one do you put out first?


* Don't malloc the stuff that doesn't need to be.
* The big nasty: threading/forking to handle simultaneous requests?
* Change the strlist implementation to operate on the head of lists instead of the tail. Gets rid of the ugly list_start junk.
* Should realloc be used instead in various places?
* Add timestamps to log printouts
