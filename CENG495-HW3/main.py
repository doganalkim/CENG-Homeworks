from parse_input import generate_chunks
import json
from celery import chain, group, chord
from app import combine_final_results, map_total_and_avg_duration, reduce_total_and_avg_duration, map_explicit_popularity, reduce_explicit_popularity, map_artist_popularity, reduce_artist_popularity, map_danceability, reduce_danceability

FILE_PATH = "./900k Definitive Spotify Dataset.json"

def main():
    chunks = list(generate_chunks(FILE_PATH))

    total_and_avg_duration = chord([map_total_and_avg_duration.s(chunk) for chunk in chunks], reduce_total_and_avg_duration.s())
    explicit_popularity = chord([map_explicit_popularity.s(chunk) for chunk in chunks], reduce_explicit_popularity.s())
    artist_popularity = chord([map_artist_popularity.s(chunk) for chunk in chunks], reduce_artist_popularity.s())
    avg_danceability_by_year = chord([map_danceability.s(chunk) for chunk in chunks], reduce_danceability.s())

    workflow = chain(
        group( total_and_avg_duration, explicit_popularity, artist_popularity, avg_danceability_by_year ),
        combine_final_results.s()
    )()

    result = workflow.get()

    with open("results.json", "w") as f:
        f.write(json.dumps(result, indent = 4))

if __name__ == '__main__':
    main()
