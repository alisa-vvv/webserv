const fileUploader = document.getElementById("fileUpload");
const submitButton = document.getElementById("submitButton");
const fileNameElement = document.getElementById("fileName");

fileUploader.addEventListener("change", function ()
{
	const file = this.files[0];
	
	if (file) {
		fileNameElement.textContent = "Selected file: " + file.name;
		submitButton.hidden = false;
	} else {
		fileNameElement.textContent = "No file selected";
		submitButton.hidden = true;
	}
});

function submitWithoutReload(event)
{
	event.preventDefault(); //browser’s form submission event

	const form = document.getElementById("uploadForm");
	const uploadMessage = document.getElementById("uploadMessage");
	const formData = new FormData(form);

	fetch(form.action, {
		method: "POST",
		body: formData
	})
	.then(response => {
		if (!response.ok)
			throw new Error(`Upload failed: HTTP ${response.status}`);

		return response.text();
	})
	.then(text => {
		console.log(text);
		uploadMessage.textContent = text;
	}) //reads the response body and converts it into a JavaScript string
	.catch(error => {
		console.error(error);
		uploadMessage.textContent = "Upload failed! gg";
	});
}

//TODO: after clicking upload button, hide that button and instead show 
// 	1. upload successfull/ upload failed message (needs cgi part)
// 	2. a new button that says "Upload new file" to start over