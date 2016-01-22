### TODO
In no particular order. When everything's on fire, which one do you put out first?


* Make a response struct?
* Don't malloc the stuff that doesn't need to be.
* The big nasty: threading/forking to handle simultaneous requests?
* Change the strlist implementation to operate on the head of lists instead of the tail. Gets rid of the ugly list_start junk.
* Add info/debug/fine printouts (with compiler flags?)
