build :
	find . -name '*' -a -type d -print -exec  ./makesub {} \;

verify :
	find . -name '*' -a -type d -print -exec  ./verifysub {} \;

clean :
	@- find . -name '*' -a -type d -print -exec  ./cleansub {} \;
	@- /bin/rm */*.MM
	@- /bin/rm */*.o
	@- /bin/rm */*.i
	@- /bin/rm */mem.leak
	@- find . -name 'a.a' -exec /bin/rm {} \;
	@- find . -name 'b.b' -exec /bin/rm {} \;
	@- find . -name 'core' -exec /bin/rm {} \;
	@- find . -name '*.bak' -exec /bin/rm {} \;
	@- find . -name '*.jou' -exec /bin/rm {} \;
	@- find . -name '*.tmp' -exec /bin/rm {} \;
