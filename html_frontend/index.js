function load_dynamic(id, url) {
  const paragraph = document.getElementById(id);

  fetch(url)
    .then(response => response.text())
    .then(html => {
      // Now you have the HTML content as a string
      paragraph.innerHTML = html; // Or use other DOM manipulation methods

      const tempDiv = document.createElement('div');
      tempDiv.innerHTML = html;

      const scriptElement = tempDiv.querySelector('script');
      if (scriptElement) {
        const scriptContent = scriptElement.textContent;
        console.log(scriptContent);
        eval(scriptContent);
      }
    })
    .catch(error => {
      console.error('Error fetching data:', error);
      paragraph.textContent = 'An error occurred while loading content.';
    });
}