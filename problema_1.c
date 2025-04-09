
/**
Varianta 9 - Problema 1
7.	Intr-un sistem, pornind de la procesul __init__ (numit in continuare P0) se pornesc inca n procese fiu in lant (P1, P2, … , Pn). Stiind ca fiecare PID va fi asignat in mod incremental, ie. PID(P0)=1, PID(Pi) = PID(Pi-1) + 1, care va fi PPID-ul fiecarui proces Pi, i=1,n ?
*/


// PPID(P0) = nedefinit

// PPID(P1) = 1

// PPID(P2) = 2

// PPID(P3) = 3

// ...

// PPID(Pn) = n-1