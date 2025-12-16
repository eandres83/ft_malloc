```
Arquitectura General
			   [ g_heap (Global) ]
                                    |
            +-----------------------+-----------------------+
            |                       |                       |
       [ TINY ZONE ]          [ SMALL ZONE ]          [ LARGE ZONE ]
    (<= 128 bytes/req)     (<= 1024 bytes/req)        (> 1024 bytes)
            |                       |                       |
            v                       v                       v
    +---------------+       +---------------+       +---------------+
    |  Header Zona  |       |  Header Zona  |       |  Block Header |
    +---------------+       +---------------+       |  (mmap direct)|
    | Bloque 1      |       | Bloque 1      |       |               |
    | Bloque 2      |       | Bloque 2      |       |   PAYLOAD     |
    | ...           |       | ...           |       |               |
    +---------------+       +---------------+       +---------------+
            | (next)                | (next)                | (next)
            v                       v                       v
    +---------------+       +---------------+             NULL
    |  Header Zona  |       |  Header Zona  |
    | (extend_zone) |       | (extend_zone) |
    +---------------+       +---------------+


Anatomia de un Bloque

Puntero interno (t_block *)            Puntero para el Usuario (void *)
    (Lo que tú manejas)                    (Lo que devuelve malloc)
            |                                       |
            v                                       v
    +-----------------------+-----------------------------------------------+
    |   METADATOS (Header)  |              PAYLOAD (Datos)                  |
    |  size, next, free...  |           "Espacio para escribir"             |
    |      (32 bytes)       |             (size solicitado)                 |
    +-----------------------+-----------------------------------------------+
    ^                       ^
    |                       |
Dirección Base          Dirección Base + 32 bytes (sizeof(t_block))
(0x1000)                (0x1020)

Malloc: Splitting

ANTES:
+-----------------------------------------------------------------------+
|                       BLOQUE LIBRE (500 bytes)                        |
+-----------------------------------------------------------------------+

DESPUÉS (split_block):
+-----------------------+   +-------------------------------------------+
| OCUPADO (100 bytes)   |   |        NUEVO LIBRE (400 bytes)            |
| (Para el usuario)     |   |          (Reciclado/Sobrante)             |
+-----------------------+   +-------------------------------------------+

Free: Coalescing 

ANTES:
+--------------+     +--------------+     +--------------+
|   LIBRE (A)  |  |  |   LIBRE (B)  |  |  |   LIBRE (C)  |
|  (Izquierda) |  |  |  (El tuyo)   |  |  |  (Derecha)   |
+--------------+     +--------------+     +--------------+

DESPUÉS (coalesce_block):
+-----------------------------------------------------------------------+
|                     GRAN BLOQUE LIBRE (A + B + C)                     |
|                   (Listo para una petición grande)                    |
+-----------------------------------------------------------------------+

```
