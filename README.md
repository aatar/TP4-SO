# TP4-SO

Lista de cosas que faltarian:
- Que no haya solo 5 asientos por vuelo, sino que sea con filas y columnas (habria que cambiar la estructura de la tabla de BD Flights)
- Consultar para qué tendríamos que implementar threads, si con procesos hijos ya estamos bien
- Hacer TDD

Consultas a hacer:
- Que parte del programita va del lado del servidor, si cada vez que manda mensaje o solamente al acceder a la bd
- Es necesario implementar threads para el servidor, dado que se puede resolver sin los mismos simplemente con forks y que haya muchos hijos escuchando a distintos clientes? Cuantos tendrían que ser los threads?(escuchar, acceso bd,...)
- Hay que hacer tdd?
- Está bien como hicimos el menú del usuario o hay que hacerlo de otra forma?
- Es normal que pase que el socket no conecta de una, y hay que probar varias veces? Si es porque dejé muchos sockets abiertos, cómo hago para cerrarlos?
- Se van a poner muy detallistas con las validaciones de la interfaz de usuario, o lo que mas miran es el socket?
- Hice que el avion tenga 5 asientos como un ejemplo, esta bien?
- Donde puedo meter los wait de los procesos hijos? Por ahora hice que tenga que alcanzar el maximo de clientes para hacer el wait
