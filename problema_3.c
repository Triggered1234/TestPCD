/**
Varianta 9 - Problema 3
2.	Ce se intampla daca procesul parinte care a creat un thread cu pthread_create() se opreste inaintea terminarii thread-ului respectiv ?
*/

//Daca procesul parinte care a creat un thread cu pthread_create() se opreste inainte ca thread-ul sa termine, thread-ul va continua
// sa ruleze. La finalizarea sa, resursele vor fi eliberate, iar procesul care a creat thread-ul devine orfan, fiind preluat de
// procesul init (PID 1).