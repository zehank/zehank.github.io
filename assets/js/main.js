/* =========================================================
   Zehan Ruan — personal site
   Vanilla JS. No dependencies.
   ========================================================= */
(function () {
  "use strict";

  var root = document.documentElement;

  /* ---------------------------------------------------------
     Language (EN default, ZH persisted)
     --------------------------------------------------------- */
  var LANG_KEY = "zr-lang";

  function currentLang() {
    return root.getAttribute("data-lang") === "zh" ? "zh" : "en";
  }

  // Assigned by the lightbox section once it is wired up. It stays a no-op until
  // then so that applyLang() is safe to call during startup — referencing the
  // lightbox directly here would throw, because it is looked up further down.
  var onLangChange = function () {};

  function applyLang(lang) {
    root.setAttribute("data-lang", lang);
    root.setAttribute("lang", lang === "zh" ? "zh-CN" : "en");
    try { localStorage.setItem(LANG_KEY, lang); } catch (e) {}
    onLangChange();
  }

  // English is the primary language; Chinese is opt-in and remembered.
  var saved = null;
  try { saved = localStorage.getItem(LANG_KEY); } catch (e) {}
  applyLang(saved === "zh" ? "zh" : "en");

  var langToggle = document.getElementById("langToggle");
  if (langToggle) {
    langToggle.addEventListener("click", function () {
      applyLang(currentLang() === "en" ? "zh" : "en");
    });
  }

  /* ---------------------------------------------------------
     Header state + mobile menu
     --------------------------------------------------------- */
  var header = document.getElementById("siteHeader");
  var nav = document.getElementById("nav");
  var menuToggle = document.getElementById("menuToggle");

  function onScroll() {
    if (!header) return;
    header.classList.toggle("is-scrolled", window.scrollY > 8);
  }
  onScroll();
  window.addEventListener("scroll", onScroll, { passive: true });

  if (menuToggle && nav) {
    menuToggle.addEventListener("click", function () {
      var open = nav.classList.toggle("is-open");
      menuToggle.setAttribute("aria-expanded", open ? "true" : "false");
    });
    nav.addEventListener("click", function (e) {
      if (e.target.closest("a")) {
        nav.classList.remove("is-open");
        menuToggle.setAttribute("aria-expanded", "false");
      }
    });
  }

  /* ---------------------------------------------------------
     Reveal on scroll
     --------------------------------------------------------- */
  var revealTargets = document.querySelectorAll(
    ".section-head, .project, .card, .award, .tl-item, .skill-card, " +
    ".vision-list li, .vision-note, .contact-card, .hero-stats, " +
    ".about-text > *, .about-card, .awards-extra"
  );

  if ("IntersectionObserver" in window) {
    var io = new IntersectionObserver(function (entries) {
      entries.forEach(function (entry) {
        if (entry.isIntersecting) {
          entry.target.classList.add("is-in");
          io.unobserve(entry.target);
        }
      });
    }, { rootMargin: "0px 0px -8% 0px", threshold: 0.06 });

    Array.prototype.forEach.call(revealTargets, function (el, i) {
      el.classList.add("reveal");
      el.style.transitionDelay = Math.min(i % 6, 5) * 45 + "ms";
      io.observe(el);
    });
  }

  /* ---------------------------------------------------------
     Lightbox
     --------------------------------------------------------- */
  var lb = document.getElementById("lightbox");
  var lbImage = document.getElementById("lbImage");
  var lbCaption = document.getElementById("lbCaption");
  var lbCounter = document.getElementById("lbCounter");

  if (lb) {
    var triggers = Array.prototype.slice.call(document.querySelectorAll("[data-src]"));
    var groups = {};
    var activeGroup = null;
    var activeIndex = 0;
    var lastFocused = null;

    triggers.forEach(function (el, i) {
      var holder = el.closest("[data-group]");
      var key = holder ? holder.getAttribute("data-group") : "__solo-" + i;
      (groups[key] = groups[key] || []).push(el);
    });

    function captionFor(el) {
      var en = el.getAttribute("data-cap-en") || "";
      var zh = el.getAttribute("data-cap-zh") || "";
      if (currentLang() === "zh") return zh || en;
      return en || zh;
    }

    function altFor(el) {
      var img = el.querySelector("img");
      return img ? (img.getAttribute("alt") || "") : "";
    }

    function renderCaption() {
      var el = groups[activeGroup][activeIndex];
      lbCaption.textContent = captionFor(el);
      lbCounter.textContent = (activeIndex + 1) + " / " + groups[activeGroup].length;
    }

    // keep an already-open lightbox caption in sync with the language.
    // Guard on `hidden`, not the `is-open` class: the class is added inside a
    // requestAnimationFrame, so it is still absent during the same tick.
    onLangChange = function () {
      if (!lb.hidden) renderCaption();
    };

    function show(groupKey, index) {
      activeGroup = groupKey;
      var list = groups[groupKey];
      activeIndex = (index + list.length) % list.length;
      var el = list[activeIndex];
      var src = el.getAttribute("data-src");

      lbImage.src = src;
      lbImage.alt = altFor(el);
      renderCaption();

      var many = list.length > 1;
      lb.querySelector("[data-prev]").hidden = !many;
      lb.querySelector("[data-next]").hidden = !many;
    }

    function open(groupKey, index, opener) {
      lastFocused = opener || null;
      lb.hidden = false;
      requestAnimationFrame(function () { lb.classList.add("is-open"); });
      document.body.classList.add("lb-open");
      show(groupKey, index);
      lb.querySelector(".lb-close").focus();
    }

    function close() {
      lb.classList.remove("is-open");
      document.body.classList.remove("lb-open");
      lb.hidden = true;
      lbImage.removeAttribute("src");
      if (lastFocused && lastFocused.focus) lastFocused.focus();
    }

    function step(dir) {
      if (!activeGroup) return;
      show(activeGroup, activeIndex + dir);
    }

    triggers.forEach(function (el) {
      el.addEventListener("click", function () {
        var holder = el.closest("[data-group]");
        var key = holder ? holder.getAttribute("data-group") : null;
        if (!key) {
          // standalone image: rebuild its solo key
          var i = triggers.indexOf(el);
          key = "__solo-" + i;
          groups[key] = groups[key] || [el];
        }
        var list = groups[key];
        open(key, list.indexOf(el), el);
      });
    });

    lb.addEventListener("click", function (e) {
      if (e.target.closest("[data-close]")) close();
      else if (e.target.closest("[data-prev]")) step(-1);
      else if (e.target.closest("[data-next]")) step(1);
    });

    document.addEventListener("keydown", function (e) {
      if (lb.hidden) return;
      if (e.key === "Escape") close();
      else if (e.key === "ArrowLeft") step(-1);
      else if (e.key === "ArrowRight") step(1);
    });
  }

  /* ---------------------------------------------------------
     Inline game demo (loaded on demand)
     --------------------------------------------------------- */
  var demoBtn = document.getElementById("demoLoad");
  var demoFrame = document.getElementById("demoFrame");

  if (demoBtn && demoFrame) {
    demoBtn.addEventListener("click", function () {
      demoFrame.hidden = false;
      demoFrame.innerHTML = "";
      var frame = document.createElement("iframe");
      frame.src = "demos/sanguosha/index.html";
      frame.title = "SanGuoSha simplified browser demo";
      frame.loading = "lazy";
      demoFrame.appendChild(frame);
      demoBtn.hidden = true;
    });
  }

  /* ---------------------------------------------------------
     Footer year
     --------------------------------------------------------- */
  var y = String(new Date().getFullYear());
  var yearEl = document.getElementById("year");
  if (yearEl) yearEl.textContent = y;
  var zhYear = document.querySelector(".year-zh");
  if (zhYear) zhYear.textContent = y;

})();
