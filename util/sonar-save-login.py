import keyring
from keyrings.alt.file import PlaintextKeyring
import os

if 'SONAR_LOGIN' not in os.environ:
    print("Unable to get SONAR_LOGIN from environment variables.")
    exit(1)

secret = os.environ['SONAR_LOGIN']
keyring.set_keyring(PlaintextKeyring())
keyring.set_password("sonar", "login", secret)
