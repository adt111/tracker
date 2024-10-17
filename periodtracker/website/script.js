// script.js

document.addEventListener('DOMContentLoaded', () => {
    const cycleForm = document.getElementById('cycleForm');
    const messageDiv = document.getElementById('message');

    cycleForm.addEventListener('submit', (event) => {
        event.preventDefault(); // Prevent form submission

        const startDate = document.getElementById('startDate').value;
        const endDate = document.getElementById('endDate').value;
        const symptoms = document.getElementById('symptoms').value.split(',').map(symptom => symptom.trim());

        // Simple validation
        if (!startDate || !endDate) {
            messageDiv.textContent = "Please enter both start and end dates.";
            messageDiv.className = 'message error';
            return;
        }

        // Create message to display
        const message = `Cycle added: Start Date: ${startDate}, End Date: ${endDate}, Symptoms: ${symptoms.join(', ')}`;
        messageDiv.textContent = message;
        messageDiv.className = 'message success';

        // Reset the form
        cycleForm.reset();
    });
});
