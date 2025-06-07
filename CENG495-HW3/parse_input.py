import json

def generate_chunks(file_path, chunk_size = 2000):

    chunk = []

    with open(file_path, "r", encoding="utf-8") as f:
        for line in f:
            if line.strip(): 
                try:
                    item = json.loads(line)
                    chunk.append(item)

                    if len(chunk) == chunk_size:
                        yield chunk
                        chunk = []

                except json.JSONDecodeError as e:
                    print(f"Skipping invalid JSON line: {e}")
    if chunk: 
        yield chunk

