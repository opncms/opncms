#!/usr/bin/env python

import requests
import json

def main():
	url = "http://localhost:8090/rpc/"
	#api_key = "be4271664ca8169d32af49d8e1ec854edb0290bc3588a2e356275eab9505"
	headers = {"content-type": "application/json"}

	payload = {
		"method": "createTask",
		"params": {
			"title": "Python API test",
			"project_id": 1
		},
		"jsonrpc": "2.0",
		"id": 1,
	}

	response = requests.post(
		url,
		data=json.dumps(payload),
		headers=headers,
		#auth=("jsonrpc", api_key)
	)

	if response.status_code == 401:
		print "Authentication failed"
	else:
		result = response.json()

	assert result["result"] == True
	assert result["jsonrpc"]
	assert result["id"] == 1

	print "Task created successfully!"

if __name__ == "__main__":
	main()
