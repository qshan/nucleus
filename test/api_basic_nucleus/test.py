from test_case_helpers import ctest, test_case_start, test_case_end, test_case_utils
import requests
import json

test_case_start()

server = test_case_utils.get_server_url()

response = requests.get(f"{server}/api/v1/ready")

assert response.status_code == 200, f"Got {response.status_code}"

assert "data" in response.json(), f"Got {json.dumps(response.json())}"

test_case_end()
