const uploadPath = "/upload/";
const uploadedFilesList = document.getElementById("uploadedFiles");
const uploadStatus = document.getElementById("uploadStatus");
const refreshUploadsButton = document.getElementById("refreshUploadsButton");

function setStatus(message, isError) {
	uploadStatus.textContent = message;
	uploadStatus.style.color = isError ? "#9e1b1b" : "#ffffff";
}

function getDeleteTarget(fileName) {
	return uploadPath + encodeURIComponent(fileName);
}

async function showServerError(response) {
	const errorHtml = await response.text();
	document.open();
	document.write(errorHtml);
	document.close();
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

			if (!response.ok) {
				await showServerError(response);
				return;
			}

			button.closest("li")?.remove();
			setStatus(`Deleted ${fileName}`, false);
			try {
				await loadUploadedFiles();
			} catch (error) {
				console.error("Could not refresh uploaded files", error);
				setStatus(`Deleted ${fileName}. Refresh failed.`, true);
			}
		} catch (error) {
			button.disabled = false;
		}
	});

	return button;
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

function createFileItem(fileName) {
	const item = document.createElement("li");
	item.className = "uploaded-file-item";

	const link = document.createElement("a");
	link.href = uploadPath + encodeURIComponent(fileName);
	link.textContent = fileName;
	item.appendChild(link);
	item.appendChild(createDeleteButton(fileName));
	return item;
}

async function loadUploadedFiles() {
	setStatus("Loading files...", false);
	uploadedFilesList.innerHTML = "";

	try {
		const response = await fetch(uploadPath, { method: "GET" });
		if (!response.ok)
			throw new Error(`HTTP ${response.status}`);

		const fileNames = parseFileNamesFromAutoindex(await response.text());
		fileNames.sort((a, b) => a.localeCompare(b));
		fileNames.forEach((fileName) => {
			uploadedFilesList.appendChild(createFileItem(fileName));
		});

		setStatus(
			fileNames.length === 0
				? "No uploaded files found."
				: `Found ${fileNames.length} file(s).`,
			false
		);
	} catch (error) {
		console.error(error);
		setStatus("Could not load uploaded files.", true);
	}
}

loadUploadedFiles();
refreshUploadsButton.addEventListener("click", loadUploadedFiles);