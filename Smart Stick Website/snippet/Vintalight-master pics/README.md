# Vintalight

Vintalight es una galería de imágenes responsive estilo fotografía instantánea con efecto lightbox creada con pug, sass y vanilla JS (Babel).
Actualmente se encuentra en su versión 2.0.

**[Demo](https://codepen.io/Jopzik/pen/zZeqRz)** | 
**[Descargar](https://github.com/jopzik/vintalight/archive/master.zip)**

## ¿Cómo se usa?

Para facilitar su implementación, se recomienda utilizar su versión de desarrollo, por lo que no se mostrará cómo implementar el código con HTML y CSS. Lo primordial es que el contenedor padre deberá ser un 
section con id y clase **vintalight**. 

### Agregar imagen
Las imágenes son agregadas mediante un mixin nombrado **vintalight**, dicho mixin recibirá como parámetros la ruta de la imagen y una 
descripción. No te preocupes por la forma o las dimensiones de la imagen, ya que se adaptará sin importar sus características, aunque 
se recomienda una resolución mínima de 400px.

#### Mixin
```PUG
mixin vintalight(img, description)
  figure.vintalight__container
      .vintalight__photo
          img(src=img, alt=description).vintalight__img
      figcaption.vintalight__caption
          h3.vintalight__text= description
```

#### Ejemplo

Donde el primer parámetro es el directorio donde se localiza la imagen y el segundo la descripción de la imagen.

```PUG
section.vintalight#vintalight
  +vintalight(photoOne.jpg, "descripciónUno")
  +vintalight(photoTwo.jpg, "descripciónDos");
```

### Posicionar imagen
Las imágenes son posicionadas mediante un mixin de sass y utilizando la pseudo clase **nth-child**.

#### Mixin

```scss
@mixin rotate($rotacion, $z-index, $coordenadaX, $coordenadaY) {
    transform: translate($coordenadaX, $coordenadaY) rotate($rotacion);
    z-index: $z-index;
    &:hover {
        transform: scale(1.15) translate($coordenadaX, $coordenadaY) rotate(0);
    }
}
```

#### Ejemplo

Donde **n** es la fotografía que quieres posicionar, **rotacion** los grados que rotará la imagen, **z-index** qué tan arriba se encontrará la 
imagen, **X** las coordenadas en X y **Y** las coordenadas en Y.

```scss
.vintalight {
    &__container {
        &:nth-child(n) {
            @include rotate(rotacion, z-index, X, Y);
        }
    }
}
```

### Variables SASS

```scss
$size: 30px; // Tamaño
$resize: 0.7; // Tamaño versión móvil
$fig-width: 10.5em; // Ancho contenedor fotografía
$fig-height: 12.25em; // Alto contenedor fotografía
$photo-size: 9.25em; // Tamaño fotografía
$font-size: 1em; // Tamaño fuente
$font-view: "Roboto", sans-serif; // Fuente pseudo elemento
$font-description: 'Nanum Pen Script', cursive; // Fuente fotografía
```

**Nota: no olvides eliminar los estilos por defecto del body en producción.**

¿Quieres ayudar a mejorar el código? Envíame un mensaje privado a mi cuenta de [twitter](https://twitter.com/Jopzik) o crea un pull request. :smile:
