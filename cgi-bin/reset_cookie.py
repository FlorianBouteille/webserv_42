#!/usr/bin/env python3
# -*- coding: utf-8 -*-

html = """<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="refresh" content="2;url=/cgi-bin/welcome.py">
    <title>Cookie réinitialisé</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 0;
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
            color: white;
            text-align: center;
        }
        .container {
            background: rgba(255, 255, 255, 0.1);
            padding: 50px;
            border-radius: 20px;
            box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.37);
        }
        h1 {
            font-size: 3em;
            margin: 0 0 20px 0;
        }
        p {
            font-size: 1.2em;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>✅ Cookie supprimé !</h1>
        <p>Redirection en cours...</p>
    </div>
</body>
</html>"""

# Delete cookie by setting Max-Age=0
import sys
sys.stdout.write("Content-Type: text/html\r\n")
sys.stdout.write("Set-Cookie: visited=0; Max-Age=0; Path=/\r\n")
sys.stdout.write("\r\n")
sys.stdout.write(html)
