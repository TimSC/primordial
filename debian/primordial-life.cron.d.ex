#
# Regular cron jobs for the primordial-life package
#
0 4	* * *	root	[ -x /usr/bin/primordial-life_maintenance ] && /usr/bin/primordial-life_maintenance
