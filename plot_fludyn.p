set terminal gif animate delay 5
FILES = system("ls outdat/*")
TITLES = system("ls outdat/* | sed 's|.*/|Time(s)=|'")
set output 'animation.gif'
set xrange [0:1000]
set yrange [0:30]
set xlabel "x (m)"
set ylabel "rho (a.u.)"
set title "Rho vs. x"

xrow = 1
xcol = 1

do for [i=1:words(FILES)]{
	plot word(FILES,i) using 1:2 with lines title word(TITLES,i)
	set xtics auto nomirror 
}

# 0:0:1:i:1:i
