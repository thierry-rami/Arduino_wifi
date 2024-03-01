// Fonction pour effectuer une requête GET à l'URL spécifiée
function fetchData() {
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            // Mettez à jour votre interface utilisateur avec les données si nécessaire
            console.log('Data:', data);
        })
        .catch(error => console.error('Error fetching data:', error));
}

// Appeler la fonction fetchData lors du chargement de la page
window.onload = function() {
    fetchData();

    // Rafraîchir les données toutes les 5 minutes (300000 millisecondes)
    setInterval(fetchData, 300000);
};
