document.getElementById('fileUpload').addEventListener('change', function()
{
	const submitButton = document.getElementById("submitButton");
	const file = this.files[0];
	var fileName = document.getElementById("fileName");

	submitButton.hidden = !file;

	if (file.textContent == file)
		fileNameElement.textContent = "Selected file: " + file.name;
	else
		fileNameElement.textContent = "No file selected";

	document.getElementById('fileName').textContent = 'Selected file: ' + fileName;
});