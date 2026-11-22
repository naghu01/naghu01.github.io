// ===== MENU HAMBURGER =====
const hamburgerIcon = document.getElementById("hamburger-icon");
const menuLinks = document.getElementById("menu-links");

function setMenu(open) {
  menuLinks.classList.toggle("open", open);
  hamburgerIcon.classList.toggle("open", open);
  hamburgerIcon.setAttribute("aria-expanded", String(open));
  hamburgerIcon.setAttribute("aria-label", open ? "Fermer le menu" : "Ouvrir le menu");
}

if (hamburgerIcon && menuLinks) {
  hamburgerIcon.addEventListener("click", () => {
    setMenu(!menuLinks.classList.contains("open"));
  });

  // Ferme le menu au clic sur un lien
  menuLinks.querySelectorAll("a").forEach((link) => {
    link.addEventListener("click", () => setMenu(false));
  });

  // Ferme le menu au clic en dehors ou avec Échap
  document.addEventListener("click", (e) => {
    if (!e.target.closest(".navbar")) setMenu(false);
  });
  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape") setMenu(false);
  });
}

// ===== LIEN ACTIF SELON LA SECTION VISIBLE =====
const navLinks = document.querySelectorAll('.nav-links a[href^="#"]');
const sections = document.querySelectorAll("main section[id]");

const observer = new IntersectionObserver(
  (entries) => {
    entries.forEach((entry) => {
      if (!entry.isIntersecting) return;
      const id = entry.target.id;
      navLinks.forEach((link) => {
        link.classList.toggle("active", link.getAttribute("href") === `#${id}`);
      });
    });
  },
  // Une section est "active" quand elle traverse le milieu de l'écran
  { rootMargin: "-45% 0px -50% 0px" }
);

sections.forEach((section) => observer.observe(section));
