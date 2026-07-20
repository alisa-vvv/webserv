const button = document.querySelector('#confetti-button');
const canvas = document.querySelector('#confetti-canvas');
const regularbutton = document.querySelector('#regular-button');

const jsConfetti = new JSConfetti();

button.addEventListener('click', async() => {
	try {
		const response = await fetch('/confetti.py');
		if (!response.ok) {
			return;
		}
		const result = await response.text();
		console.log(result);
		jsConfetti.addConfetti({
		emojis: ['🎉', '🥳', '💥', '✨', '🫰🏽'],
	})
	}
	catch (error){
		console.error(error);
	}
})

regularbutton.addEventListener('click', () => {
	jsConfetti.addConfetti({
		emojis: ['🎉', '🥳', '💥', '✨', '🫰🏽'],
	})
})

