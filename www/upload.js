const fileUploader = document.getElementById("fileUpload");
const submitButton = document.getElementById("submitButton");
const fileNameElement = document.getElementById("fileName");

fileUploader.addEventListener("change", function ()
{
	const file = this.files[0];
	submitButton.hidden = !file;
	
	if (fileNameElement.textContent = file) {
		fileNameElement.textContent = "Selected file: " + file.name;
		submitButton.hidden = false;
	} else {
		fileNameElement.textContent = "No file selected";
		submitButton.hidden = true;
	}
});
