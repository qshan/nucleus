# TODO - Add more strings including utf-8 etc

from test_case_helpers import ctest, test_case_start, test_case_end, test_case_check, test_case_utils as utils
import requests
import json

test_case_start()

server = utils.get_server_url()

api_url = f"{server}/api/v1/app/message"
response = requests.get(api_url)
print(json.dumps(response.json(), indent=4))
assert utils.status_codes_equal(response.status_code, requests.codes.ok)
assert "value" in response.json()["data"]
assert response.json()["data"]["value"] == "Hello World"
test_case_check("Get value")

j = {"value": "Hello Back"}
response = requests.put(api_url, json=j)
assert utils.status_codes_equal(response.status_code, requests.codes.ok)
test_case_check("Put value")

response = requests.get(api_url)
assert utils.status_codes_equal(response.status_code, requests.codes.ok)
assert response.json()["data"]["value"] == "Hello Back"
test_case_check("Value ok")

j = {"value": ""}
response = requests.put(api_url, json=j)
print(json.dumps(response.json(), indent=4))
assert utils.status_codes_equal(response.status_code, requests.codes.bad_request)
test_case_check("Post bad value gives error")

j = {"missing_value": ""}
response = requests.put(api_url, json=j)
print(json.dumps(response.json(), indent=4))
assert utils.status_codes_equal(response.status_code, requests.codes.internal_server_error)
test_case_check("Post bad schema gives error")



test_case_end()
