const uploadPath = "/upload/";
const uploadedFilesList = document.getElementById("uploadedFiles");
const deleteStatus = document.getElementById("deleteStatus");

function setStatus(message, isError) {
	deleteStatus.textContent = message;
	deleteStatus.style.color = isError ? "#9e1b1b" : "#ffffff";
}

function getDeleteTarget(fileName) {
	return uploadPath + encodeURIComponent(fileName);
}

function createDeleteButton(fileName) {
	const button = document.createElement("button");
	button.textContent = "Delete";
	button.className = "btn-55 file-delete-btn";

	button.addEventListener("click", async () => {
		button.disabled = true;
		setStatus(`Deleting ${fileName}...`, false);
		try {
			const response = await fetch(getDeleteTarget(fileName), {
				method: "DELETE"
			});

			if (!response.ok)
				throw new Error(`HTTP ${response.status}`);

			setStatus(`Deleted ${fileName}`, false);
			await loadUploadedFiles();
		} catch (error) {
			button.disabled = true;
		}
	});

	return button;
}

function createFileItem(fileName) {
	const item = document.createElement("li");
	item.className = "uploaded-file-item";

	const name = document.createElement("span");
	name.textContent = fileName;
	name.className = "uploaded-file-name";

	item.appendChild(name);
	item.appendChild(createDeleteButton(fileName));
	return item;
}

function parseFileNamesFromAutoindex(htmlText) {
	const parser = new DOMParser();
	const htmlDoc = parser.parseFromString(htmlText, "text/html");
	const links = htmlDoc.querySelectorAll("a[href]");

	return Array.from(links)
		.map((link) => {
			const href = link.getAttribute("href") || "";
			return decodeURIComponent(href.replace(/\/$/, "")).trim();
		})
		.filter((name) => name.length > 0 && name !== "." && name !== "..");
}

async function loadUploadedFiles() {
	setStatus("Loading files...", false);
	uploadedFilesList.innerHTML = "";

	try {
		const response = await fetch(uploadPath, { method: "GET" });
		if (!response.ok)
			throw new Error(`HTTP ${response.status}`);

		const htmlText = await response.text();
		const fileNames = parseFileNamesFromAutoindex(htmlText);

		if (fileNames.length === 0) {
			setStatus("No uploaded files found.", false);
			return;
		}

		fileNames.sort((a, b) => a.localeCompare(b));
		fileNames.forEach((fileName) => {
			uploadedFilesList.appendChild(createFileItem(fileName));
		});
		setStatus(`Found ${fileNames.length} file(s).`, false);
	} catch (error) {
		console.error(error);
		setStatus("No files uploaded yet", true);
	}
}

loadUploadedFiles();
