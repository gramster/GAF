BEGIN { hc = 0; cc = 0; ht = 0; ct = 0; ng = 0; }
{
	hc += $2
	cc += $3
	ht += $5
	ct += $6
	ng++
}
END { print "Captures ",hc/ng,cc/ng,"Turf",ht/ng,ct/ng; }

