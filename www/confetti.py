#!/usr/bin/env python3

import random

def random_color():
    return "#{:06x}".format(random.randint(0, 0xFFFFFF))

background = random_color()
text = random_color()
button = random_color()
button_text = random_color()
card = random_color()
border = random_color()

print("Content-Type: text/html")
print()

print(f"""
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<link rel="stylesheet" href="style.css">
    <title>Webserv CGI Test</title>
    

    <style>
        * {{
            box-sizing: border-box;
        }}

        body {{
            margin: 0;
            height: 100vh;
            background-color: {background};
            color: {text};

            display: flex;
            justify-content: center;
            align-items: center;

            font-family: Arial, sans-serif;
            transition: all 0.5s ease;
        }}

        .card {{
            background-color: {card};
            border: 8px solid {border};

            padding: 50px;
            border-radius: 25px;
            text-align: center;

            max-width: 700px;
        }}

        h1 {{
            font-size: 45px;
        }}

        p {{
            font-size: 20px;
        }}

        button {{
            background-color: {button};
            color: {button_text};

            border: none;
            padding: 15px 30px;
            border-radius: 10px;

            font-size: 18px;
            cursor: pointer;
        }}

        button:hover {{
            transform: scale(1.1);
        }}
    </style>
    
</head>

<body>
	<div id="header">
		<h1>
			<a href="index.html" id="title">webserv</a>
		</h1>
			<h2 id="made-by">
				made by Alisa, Ally, and Tutku
			</h2>
		</div>
		<div id="navigation">
			<h2> <a href="index.html" id="nav-link">Home</a> </h2>
		</div>
    <div class="card">
        <h1> CGI WORKS!</h1>

        <p>
            Every execution generates random colors.
        </p>

        <p>
            Background: {background}<br>
            Text: {text}<br>
            Card: {card}<br>
            Border: {border}
        </p>

        <button onclick="location.reload()">
            Execute CGI Again
        </button>
    </div>

</body>
</html>
""")