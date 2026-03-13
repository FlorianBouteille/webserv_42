#!/usr/bin/env python3
import os
import sys
from datetime import datetime

# Récupérer les variables d'environnement CGI
method = os.environ.get('REQUEST_METHOD', 'GET')
query_string = os.environ.get('QUERY_STRING', '')
content_length = os.environ.get('CONTENT_LENGTH', '0')

# Lire le body si POST
body = ""
if method == "POST" and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))

# Construire la réponse HTTP
response = f"""HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\r\n\r\n
<!DOCTYPE html>
<html>
<head>
    <title>CGI Test</title>
</head>
<body>
    <h1>CGI Script Works!</h1>
    <p>Current time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
    
    <h2>Request Info:</h2>
    <ul>
        <li><strong>Method:</strong> {method}</li>
        <li><strong>Query String:</strong> {query_string if query_string else '(empty)'}</li>
        <li><strong>Content-Length:</strong> {content_length}</li>
    </ul>
    
    {f'<h2>Body Received:</h2><pre>{body}</pre>' if body else ''}
    
    <p><a href="/cgi-bin/test.py">Back</a></p>
</body>
</html>
"""

# Envoyer la réponse
print(response, end='')
sys.exit(0)
