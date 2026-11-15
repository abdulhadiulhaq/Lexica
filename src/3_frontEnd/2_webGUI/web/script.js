document.addEventListener("DOMContentLoaded", () => {
    const searchInput           = document.getElementById("searchInput");
    const searchBtn             = document.getElementById("searchBtn");
    const topKSelect            = document.getElementById("topKSelect");
    const resultsContainer      = document.getElementById("resultsContainer");
    const autocompleteDropdown  = document.getElementById("autocompleteDropdown");
    const cacheBadge            = document.getElementById("cacheBadge");
    const fuzzyAlert            = document.getElementById("fuzzyAlert");
    const statDocs              = document.getElementById("statDocs");
    const statTerms             = document.getElementById("statTerms");

    // Fetch initial stats
    fetchStats();

    async function fetchStats() {
        try {
            const res = await fetch("/api/stats");
            const data = await res.json();
            statDocs.textContent = data.documents;
            statTerms.textContent = data.uniqueTerms;
        } catch (e) {
            console.error("Failed to fetch stats", e);
        }
    }

    // Autocomplete listener
    searchInput.addEventListener("input", async (e) => {
        const val = e.target.value.trim();
        if (val.length === 0) {
            autocompleteDropdown.innerHTML = "";
            autocompleteDropdown.style.display = "none";
            return;
        }

        const lastWord = val.split(" ").pop();
        if (lastWord.length < 1) {
            autocompleteDropdown.style.display = "none";
            return;
        }

        try {
            const res = await fetch(`/api/suggest?q=${encodeURIComponent(lastWord)}`);
            const data = await res.json();
            
            if (data.suggestions && data.suggestions.length > 0) {
                autocompleteDropdown.innerHTML = "";
                data.suggestions.forEach(s => {
                    const item = document.createElement("div");
                    item.className = "dropdown-item";
                    item.innerHTML = `<span>${s.term}</span> <span class="freq">(${s.freq})</span>`;
                    item.addEventListener("click", () => {
                        const words = searchInput.value.split(" ");
                        words[words.length - 1] = s.term;
                        searchInput.value = words.join(" ") + " ";
                        autocompleteDropdown.style.display = "none";
                        searchInput.focus();
                    });
                    autocompleteDropdown.appendChild(item);
                });
                autocompleteDropdown.style.display = "block";
            } else {
                autocompleteDropdown.style.display = "none";
            }
        } catch (err) {
            console.error("Autocomplete error", err);
        }
    });

    // Hide dropdown on click outside
    document.addEventListener("click", (e) => {
        if (!e.target.closest(".search-box-wrapper")) {
            autocompleteDropdown.style.display = "none";
        }
    });

    // Execute Search
    async function performSearch() {
        const query = searchInput.value.trim();
        if (!query) return;
        autocompleteDropdown.style.display = "none";
        fuzzyAlert.classList.add("hidden");

        const topK = topKSelect.value;

        try {
            const res = await fetch(`/api/search?q=${encodeURIComponent(query)}&k=${topK}`);
            const data = await res.json();

            // Cache badge handling
            if (data.cacheHit) {
                cacheBadge.textContent = "Cache: HIT";
                cacheBadge.className = "badge hit";
            } else {
                cacheBadge.textContent = "Cache: MISS";
                cacheBadge.className = "badge miss";
            }

            resultsContainer.innerHTML = "";

            if (data.results && data.results.length > 0) {
                data.results.forEach((r, index) => {
                    const card = document.createElement("div");
                    card.className = "result-card";
                    card.innerHTML = `
                        <div class="result-header">
                            <span class="result-rank">#${index + 1}</span>
                            <span class="result-filename">${r.fileName}</span>
                            <span class="result-score">Score: ${r.score.toFixed(4)}</span>
                        </div>
                    `;
                    resultsContainer.appendChild(card);
                });
            } else {
                resultsContainer.innerHTML = `<div class="placeholder-state">No matching documents found.</div>`;
                
                // Check fuzzy match suggestions
                checkFuzzyMatch(query);
            }
        } catch (err) {
            console.error("Search error", err);
        }
    }

    async function checkFuzzyMatch(term) {
        const firstWord = term.split(" ")[0];
        try {
            const res = await fetch(`/api/fuzzy?q=${encodeURIComponent(firstWord)}`);
            const data = await res.json();
            if (data.found && data.distance > 0 && data.distance <= 3) {
                fuzzyAlert.innerHTML = `Did you mean: <strong>${data.bestMatch}</strong> (distance ${data.distance})?`;
                fuzzyAlert.classList.remove("hidden");
            }
        } catch (e) {
            console.error("Fuzzy check error", e);
        }
    }

    searchBtn.addEventListener("click", performSearch);
    searchInput.addEventListener("keypress", (e) => {
        if (e.key === "Enter") performSearch();
    });
});