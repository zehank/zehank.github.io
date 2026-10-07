# zehank.github.io

Personal website of **Zehan Ruan** — first-year Electrical Engineering student at the
University of Waterloo.

Hand-written HTML, CSS and JavaScript. No framework, no build step, no dependencies.
Bilingual (English / 中文) with a toggle in the header.

## Structure

```
index.html                 the whole site (one page)
assets/css/style.css       styles
assets/js/main.js          language toggle, lightbox, scroll reveal, demo loader
assets/img/                optimised images (EXIF stripped)
assets/files/              resume PDF, poster PDF, C++ source
demos/sanguosha/           playable HTML5 canvas prototype
.nojekyll                  serve files as-is on GitHub Pages
```

## Local preview

```sh
python3 -m http.server 8000
# then open http://localhost:8000/
```

## Deploy

Served by GitHub Pages from the `main` branch root of this repository.
