const button = document.querySelector('#confetti-button');
const canvas = document.querySelector('#confetti-canvas');
const regularbutton = document.querySelector('#regular-button');

const jsConfetti = new JSConfetti();


regularbutton.addEventListener('click', () => {
	jsConfetti.addConfetti({
		emojis: ['🎉', '🥳', '💥', '✨', '🫰🏽'],
	})
})

