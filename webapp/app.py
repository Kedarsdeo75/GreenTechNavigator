from flask import Flask, render_template, request, redirect, url_for, Response, stream_with_context, send_from_directory, jsonify
from dotenv import set_key, load_dotenv
from flask_socketio import SocketIO, emit
import os
import subprocess
import threading
import time
import re
from flask import render_template
import sys


app = Flask(__name__)
socketio = SocketIO(app)
process_running = False
lock = threading.Lock()

# Load the .env file
dotenv_path = '/app/.env'
load_dotenv(dotenv_path)

# Helper function to format paths for Windows
def format_path(path):
    return path.replace("/", "\\").replace("\\", "\\")

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/save', methods=['POST'])
def save_config():

    azure_model = request.form['azure_model']
    report_path = request.form['report_path']
    
    # Prompts configuration
    prompts = {
        "PROMPT_1": request.form.get('prompt_1', 'n'),
        "PROMPT_2": request.form.get('prompt_2', 'n'),
        "PROMPT_3": request.form.get('prompt_3', 'n'),
        "PROMPT_4": request.form.get('prompt_4', 'n'),
        "PROMPT_5": request.form.get('prompt_5', 'n'),
        "PROMPT_6": request.form.get('prompt_6', 'n'),
        "PROMPT_7": request.form.get('prompt_7', 'n'),
        "PROMPT_GENERATE_TESTCASES": request.form.get('prompt_generate_testcases', 'n')
    }

    # Save to .env
    set_key(dotenv_path, "AZURE_MODEL", azure_model)
    set_key(dotenv_path, "REPORT_PATH", report_path)
    
    # Save prompt configurations
    for prompt, value in prompts.items():
        env_value = os.getenv(prompt, "")
        prompt_text = env_value.split(",")[0] if env_value else "Default Text"
        set_key(dotenv_path, prompt, f"{prompt_text}, {value}")

    return redirect(url_for('index'))

@app.route('/run', methods=['POST'])
def run_code_refiner():
        # Create the flag file to signal the entrypoint script
    with open('/app/run_scripts.flag', 'w') as f:
        f.write('run')
    return render_template("running.html")

@app.route('/stream')
def stream():
    global process_running

    def generate():
        global process_running
        with lock:
            if process_running:
                yield "data: Process already running\n\n"
                return
            process_running = True

        try:
            with subprocess.Popen(
                ["/bin/bash", "/app/entrypoint.sh"],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1
            ) as process:
                for line in iter(process.stdout.readline, ''):
                    print(line.strip(), flush=True)
                    yield f"data: {line}\n\n"
                process.stdout.close()
                process.wait()
        finally:
            with lock:
                process_running = False

    return Response(generate(), mimetype='text/event-stream')


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
