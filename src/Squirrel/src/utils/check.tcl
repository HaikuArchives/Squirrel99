

while {1} {

	set ps [exec ps -uacx | grep Squirrel]

	puts "mem = [lindex $ps 3]"
}


