import RPi.GPIO as GPIO
import time
import sys

DOT         = 1
DASH        = 4
CHAR_GAP    = 2
WORD_GAP    = 6
TERMINATE   = 8

class ArgumentError(Exception):
    pass

if len(sys.argv) != 3:
    raise ArgumentError(r'Error: Invalid number of arguments. Please enter initiate this program like `sudo python3 LED.py <NUMBER OF MESSAGES SENT> "<MESSAGE>"` ')

class morse_translator():
    def __init__(self, message, message_number = 1):
        self.morse_code_dict = {
            'A': '.-'   ,
            'B': '-...' ,
            'C': '-.-.' ,
            'D': '-..'  ,
            'E': '.'    ,
            'F': '..-.' ,
            'G': '--.'  ,
            'H': '....' ,
            'I': '..'   ,
            'J': '.---' ,
            'K': '-.-'  ,
            'L': '.-..' ,
            'M': '--'   ,
            'N': '-.'   ,
            'O': '---'  ,
            'P': '.--.' ,
            'Q': '--.-' ,
            'R': '.-.'  ,
            'S': '...'  ,
            'T': '-'    ,
            'U': '..-'  ,
            'V': '...-' ,
            'W': '.--'  ,
            'X': '-..-' ,
            'Y': '-.--' ,
            'Z': '--..' ,

            '0': '-----',
            '1': '.----',
            '2': '..---',
            '3': '...--',
            '4': '....-',
            '5': '.....',
            '6': '-....',
            '7': '--...',
            '8': '---..',
            '9': '----.'
        }
        self.morse_code_durations = {
            '.'     : DOT     ,
            '-'     : DASH    ,
            '/'     : WORD_GAP,
            ' '     : CHAR_GAP,
            '|'     : TERMINATE
        }
        self.number_of_messages = message_number
        self.message = message
        self.translation = self.translate_message()

    def translate_message(self):
        morse_translation = ''
        for word in self.message.split():
        
            for letter in word:
                morse_translation += self.morse_code_dict[letter.upper()] if letter != ' ' else ' '
                morse_translation += ' '
            
            morse_translation += '/ '
        
        morse_translation = morse_translation.rstrip(' /')
        morse_translation += '|'
        return morse_translation
    
    def send_morse_message(self):
        print(f'Sending the message "{self.message}" translated to "{self.translation}"')
        for i in range(self.number_of_messages):
        
            try:
                for letter in self.translation:
                    print(f"Current Symbol: {letter} | Intended to last: {self.morse_code_durations[letter]}")
                    GPIO.output(4, GPIO.HIGH)
                    time.sleep(self.morse_code_durations[letter])
        
                    GPIO.output(4, GPIO.LOW)
                    time.sleep(0.5)

            except KeyboardInterrupt:
                print("\nExiting translation early")
                GPIO.output(4, GPIO.LOW)
                break


    def print_translation():
        print(f'The message "{self.message}" translates to "{self.translation}"')

def init_GPIO(self, GPIO_PIN = 4):
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(GPIO_PIN, GPIO.OUT)

if __name__ == '__main__':
    init_GPIO(4)
    messager = morse_translator(sys.argv[2], int(sys.argv[1]))
    messager.send_morse_message()
