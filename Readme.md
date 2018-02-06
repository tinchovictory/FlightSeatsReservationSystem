# Final Sistemas Operativos

Sistema cliente-servidor, que simula reserva de asientos en un avion, utilizando sockets con una interfaz de usuario y permitiendo mensajes concurrentes.

## Instalacion

Se detallan los pasos a seguir para instalar el sistema utilizando la misma computadora que albergue el servidor y los clientes. Se puede ejecutar de manera remota tomando los recaudos suficientes. Es por esto que se utiliza el nombre de host **localhost** y se utiliza arbitrariamente el **puerto 8000** (se podria utilizar otro).
1. Descargar la carpeta del sistema y ubicarse en la misma 
2. Ejecutar el comando `make all` para compilar todos los archivos necesarios e instalar la base de datos. Se recomienda ejecutar previamente un `make clean` para borrar archivos precompilados que puedan traer problemas de versiones anteriores.
3. Ejecutar `./server 8000` esto iniciará el servidor en el *puerto 8000*.
4. En otra pestaña o ventana de la terminal ejecutar `./client localhost 8000` esto conectará al terminal cliente con el servidor en *puerto 8000* bajo el nombre de *localhost*.
5. Utilizar el sistema libremente siguiendo los pasos que se indican en la interfaz grafica.


---

Martin Victory, Florencia Cavallin y Martin Grabina
ITBA - 2018