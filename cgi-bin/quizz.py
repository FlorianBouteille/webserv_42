#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import urllib.parse

def parse_post_data():
    """Parse les données POST envoyées par le formulaire"""
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    except ValueError:
        content_length = 0
    
    if content_length > 0:
        post_data = sys.stdin.read(content_length)
        return urllib.parse.parse_qs(post_data)
    return {}

def calculate_score(data):
    """Calcule le score total du quizz"""
    score = 0
    
    # Additionner les valeurs de toutes les réponses
    for key in ['choice', 'qualite', 'metro', 'dej', 'rdv', 'pokemon']:
        if key in data:
            try:
                score += int(data[key][0])
            except (ValueError, IndexError):
                pass
    
    return score

def get_result(score):
    """Retourne le résultat basé sur le score"""
    results = {
        0: {
            'title': 'Tu es une chaussette trouée',
            'image': 'images/trouee.jpeg'
        },
        1: {
            'title': 'Tu es une chaussette dépareillée',
            'image': 'images/depareillee.png'
        },
        2: {
            'title': 'Tu es une chaussette qui pue',
            'image': 'images/quipue.jpg'
        },
        3: {
            'title': 'Tu es une chaussette pliée et repassée',
            'image': 'images/pliee.png'
        },
        4: {
            'title': 'Tu es une chaussette à doigts de pieds',
            'image': 'images/doigts.jpg'
        },
        5: {
            'title': 'Tu es une chaussette solitaire',
            'image': 'images/solitaire.png'
        }
    }
    
    # Utiliser le modulo pour avoir un résultat entre 0 et 5
    result_key = score % 6
    return results.get(result_key, results[0])

def generate_html(result):
    """Génère la page HTML complète avec le résultat"""
    html = f"""<!DOCTYPE html>
<html>
    <head>
		<link rel="stylesheet" href="/css/style.css">
		<meta charset="utf-8">
    	<style>
		@import url('https://fonts.googleapis.com/css2?family=Monoton&display=swap');
		</style>
	</head>
    <body>
		<header>
        	<h1 class="title">
            	Bienvenue sur le site des chaussettes !
        	</h1>
			<h2>
				Commandez chez nous pour toujours plus de webserv
			</h2>
		</header>
        <h3>Quelle chaussette es-tu ?</h3>
        <main class="quizzpage">
			<div class="quizz">
				<form method="POST" action="/cgi-bin/quizz.py">
					<h3>tu es plutôt team :</h3>
					<input type="radio" name="choice" value="1" id="Pixar">
					<label for = "Pixar">Pixar</label><br>
					<input type="radio" name="choice" value="2" id="Marvel">
					<label for = "Marvel">Marvel</label><br>
					<input type="radio" name="choice" value="3" id="Disney">
					<label for = "Disney">Disney</label><br>
					<h3>Tu es plutôt team</h3>
					<input type="radio" name="qualite" value="1" id="Star academy">
					<label for = "Star academy">Star academy</label><br>
					<input type="radio" name="qualite" value="2" id="Top chef">
					<label for = "Top chef">Top chef</label><br>
					<input type="radio" name="qualite" value="3" id="Koh Lanta">
					<label for = "Koh Lanta">Koh Lanta</label><br>
					<h3>Une vieille dame cherche une place dans le metro :</h3>
					<input type="radio" name="metro" value="1" id="metro1">
					<label for = "metro1">Tu lui laisses ta place</label><br>
					<input type="radio" name="metro" value="2" id="metro2">
					<label for = "metro2">Tu l'ignores et te plonges dans candy crush</label><br>
					<input type="radio" name="metro" value="3" id="metro3">
					<label for = "metro3">Tu as bien merité cette place, quelqu'un d'autre va bien se sacrifier</label><br>
					<h3>Pour la pause dej :</h3>
					<input type="radio" name="dej" value="1" id="dej1">
					<label for = "dej1">Tu commandes un bon ptit plat sur steakOverflow</label><br>
					<input type="radio" name="dej" value="2" id="dej2">
					<label for = "dej2">Ta bento est toute prete dans le frigo</label><br>
					<input type="radio" name="dej" value="3" id="dej3">
					<label for = "dej3">Tu es completement decale et ton hygiene de vie est deplorable, tu mangeras quand tu auras un CDI</label><br>
					<h3>Tu as un rendez-vous a 10h30 :</h3>
					<input type="radio" name="rdv" value="1" id="rdv1">
					<label for = "rdv1">Tu arrives a 10h30 pile</label><br>
					<input type="radio" name="rdv" value="2" id="rdv2">
					<label for = "rdv2">Tu es toujours en retard, tu pars de chez toi a 10h20...</label><br>
					<input type="radio" name="rdv" value="3" id="rdv3">
					<label for = "rdv3">Tu arrives a 10h, juste au cas ou</label><br>
					<h3>Quel est le meilleur starter ? </h3>
					<input type="radio" name="pokemon" value="1" id="pokemon1">
					<label for = "pokemon1">Salameche, le classique</label><br>
					<input type="radio" name="pokemon" value="2" id="pokemon2">
					<label for = "pokemon2">Carapuce, le facétieux</label><br>
					<input type="radio" name="pokemon" value="3" id="pokemon3">
					<label for = "pokemon3">Bulbizarre, l'outsider</label><br><br>
					<input type="submit" value="Resultat !">
				</form>
			</div>
			<div class="answerContainer">
				<h3>{result['title']}</h3>
				<img src="/{result['image']}" alt="{result['title']}">
			</div>
        </main>
    </body>
</html>"""
    return html

def main():
    # En-têtes HTTP CGI
    print("Content-Type: text/html; charset=utf-8")
    print()  # Ligne vide obligatoire entre en-têtes et contenu
    
    # Parser les données POST
    post_data = parse_post_data()
    
    # Si pas de données, afficher le formulaire vide
    if not post_data:
        result = {'title': '', 'image': ''}
        print(generate_html(result))
    else:
        # Calculer le score et obtenir le résultat
        score = calculate_score(post_data)
        result = get_result(score)
        
        # Générer et afficher la page HTML avec le résultat
        print(generate_html(result))

if __name__ == "__main__":
    main()
