#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import http.cookies

# Parse cookies from the request
cookie_string = os.environ.get('HTTP_COOKIE', '')
cookies = http.cookies.SimpleCookie()
if cookie_string:
    cookies.load(cookie_string)

# Check if user has visited before
has_visited = 'visited' in cookies
visit_count = 0

if has_visited:
    try:
        visit_count = int(cookies['visited'].value)
    except:
        visit_count = 0

visit_count += 1

# Generate different HTML based on visit status
if visit_count == 1:
    # First visit
    title = "🎉 Bienvenue !"
    message = "C'est votre première visite sur notre site !"
    bg_color = "#667eea"
    text = "Nous sommes ravis de vous accueillir. Un cookie a été créé pour se souvenir de vous !"
else:
    # Returning visitor
    title = "👋 Welcome Back !"
    message = f"Vous avez visité cette page {visit_count} fois !"
    bg_color = "#11998e"
    text = "Content de vous revoir ! Votre cookie fonctionne parfaitement."

html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{title}</title>
    <style>
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 0;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            background: linear-gradient(135deg, {bg_color} 0%, #764ba2 100%);
            color: white;
        }}
        .container {{
            text-align: center;
            background: rgba(255, 255, 255, 0.1);
            padding: 50px;
            border-radius: 20px;
            box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.37);
            backdrop-filter: blur(4px);
            max-width: 600px;
        }}
        h1 {{
            font-size: 3em;
            margin: 0 0 20px 0;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
        }}
        .message {{
            font-size: 1.5em;
            margin: 20px 0;
            font-weight: bold;
        }}
        .text {{
            font-size: 1.1em;
            margin: 20px 0;
            opacity: 0.9;
        }}
        .cookie-info {{
            background: rgba(255, 255, 255, 0.2);
            padding: 20px;
            border-radius: 10px;
            margin-top: 30px;
            font-size: 0.9em;
        }}
        .visit-badge {{
            display: inline-block;
            background: #ffd700;
            color: #333;
            padding: 10px 20px;
            border-radius: 25px;
            font-weight: bold;
            font-size: 1.2em;
            margin: 20px 0;
        }}
        a {{
            display: inline-block;
            margin-top: 20px;
            padding: 12px 30px;
            background: rgba(255, 255, 255, 0.3);
            color: white;
            text-decoration: none;
            border-radius: 25px;
            transition: all 0.3s;
        }}
        a:hover {{
            background: rgba(255, 255, 255, 0.5);
            transform: scale(1.05);
        }}
        button {{
            margin-top: 20px;
            padding: 12px 30px;
            background: #ff6b6b;
            color: white;
            border: none;
            border-radius: 25px;
            font-size: 1em;
            cursor: pointer;
            transition: all 0.3s;
        }}
        button:hover {{
            background: #ee5a52;
            transform: scale(1.05);
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>{title}</h1>
        <div class="message">{message}</div>
        <div class="visit-badge">Visite n°{visit_count}</div>
        <div class="text">{text}</div>
        <div class="cookie-info">
            <strong>🍪 Cookie Info:</strong><br>
            Nom: <code>visited</code><br>
            Valeur: <code>{visit_count}</code><br>
            Expiration: 1 heure
        </div>
        <form method="POST" action="/cgi-bin/reset_cookie.py" style="display: inline;">
            <button type="submit">Réinitialiser le cookie</button>
        </form>
        <br>
        <a href="/index.html">← Retour à l'accueil</a>
    </div>
</body>
</html>"""

# Send HTTP response with Set-Cookie header
import sys
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write(f"Set-Cookie: visited={visit_count}; Max-Age=3600; Path=/\r\n")
sys.stdout.write("\r\n")
sys.stdout.write(html)
