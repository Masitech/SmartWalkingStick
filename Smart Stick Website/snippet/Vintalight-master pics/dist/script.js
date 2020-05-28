"use strict";

// Función que activa el overlay. Se pasa como parámetro el contenedor padre (#vintalight en el DOM)
var activeVintalight = function activeVintalight(container) {
    // Delegación de eventos para detectar click en los hijos
    container.addEventListener("click", function (e) {
        var element = e.target;
        // Validar que se haya dado click en el pseudoelemento before
        if (element.tagName == "DIV") {
            // Obtener dirección y descripción de la imagen que se dio click
            var src = element.querySelector("img").getAttribute("src"),
                descrip = element.querySelector("img").getAttribute("alt"),

            // Crear un nuevo div que se usará como overlay
            vintalightOverlay = document.createElement("div");
            // Agregar clase al div que creamos para poder darle estilos con CSS
            vintalightOverlay.classList.add("vintalight-overlay");
            // Agregar contenido al overlay 
            vintalightOverlay.innerHTML = "\n                <figure class=\"vintalight__container active\">\n                    <div class=\"vintalight__photo\">\n                        <img src=\"" + src + "\" alt=\"" + descrip + "\" class=\"vintalight__img\"/>\n                    </div>\n                    <figcaption class=\"vintalight__caption\">\n                        <h3 class=\"vintalight__text\">" + descrip + "</h3>\n                    </figcaption>\n                    <button class=\"vintalight__button\" id=\"button-close\">\u2715</button>\n                </figure>\n            ";
            // Meter el overlay en el DOM
            document.body.appendChild(vintalightOverlay);
            // Añadimos la clase active para poder darle transición
            setTimeout(function () {
                vintalightOverlay.classList.add("active");
            }, 1);
            // Eliminar el scroll del body
            document.body.style.overflow = "hidden";
            // Evento para cerrar el overlay
            document.getElementById("button-close").addEventListener("click", function () {
                // Eliminar clase active
                vintalightOverlay.classList.remove("active");
                // Eliminar overlay del DOM
                setTimeout(function () {
                    document.body.removeChild(vintalightOverlay);
                }, 500);
                // Devolver scroll al body
                document.body.style.overflow = "auto";
            });
            // Evento para cerrar el overlay con la tecla ESC
            window.addEventListener("keyup", function (e) {
                if (e.key === "Escape") document.getElementById("button-close").click();
            });
        }
    });
};

// Activamos la función
window.addEventListener("load", activeVintalight(document.getElementById("vintalight")));