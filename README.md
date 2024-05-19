===== Software Design =====

**Descrierea codului:**
Proiectul este conceput pentru a rula un joc bazat pe timpul de reacție, utilizând un Arduino, un LCD, LED-uri pentru indicii vizuale și un buton pentru interacțiunea utilizatorului. Firmware-ul gestionează inițializarea jocului, logica jocului, interacțiunea cu utilizatorul și gestionarea scorurilor. Timpul de reacție al jucătorului este înregistrat și penalizat dacă nu se încadrează în pragul de timp așteptat.

**Mediu de dezvoltare:**
Firmware-ul a fost dezvoltat folosind Arduino IDE.

Biblioteci şi surse 3rd-party:
  * **LiquidCrystal Library**: interfațarea cu LCD-ul
  * **SD Library**: interfațarea cu un modul de card SD, pentru citirea și scrierea datelor pe un card SD

**Algoritmi şi structuri:**
  * **Selecția aleatorie a LED-urilor:** Un algoritm pentru a selecta aleatoriu unul dintre cele trei LED-uri (albastru, galben, roșu) pentru a se aprinde.
  * **Detecția apăsării butoanelor:** O structură pentru a detecta apăsările butoanelor și a determina dacă a fost apăsat butonul corect în funcție de culoarea LED-ului și zona butoanelor (STÂNGA sau DREAPTA).
  * **Calcularea timpului de reacție:** Un algoritm pentru a calcula timpul de reacție al jucătorului (suma timpilor individuali).

**Surse și funcții implementate:**
  * **setup():** Inițializează componentele hardware și afișează mesajul inițial pe LCD.
  * **loop():** Conține bucla principală a jocului, verificând apăsările butonului pentru a începe jocul sau a salva scorul.
  * **startGame():** Resetează variabilele jocului și începe jocul.
  * **playGame():** Gestionează logica principală a jocului, inclusiv aprinderea LED-urilor, detectarea apăsărilor butoanelor și înregistrarea timpilor de reacție.
  * **endGame():** Calculează timpul total și mediu de reacție, le afișează și gestionează promptul de salvare a scorului.
  * **saveScore():** Scrie scorul jucătorului pe cardul SD.
  * **getScoreRanking():** Citește scorurile de pe cardul SD și calculează clasamentul jucătorului.
