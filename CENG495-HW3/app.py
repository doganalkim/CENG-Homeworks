from celery import Celery
from datetime import datetime
from collections import defaultdict
import operator
import json

app = Celery(
    'app',
    broker='redis://localhost:6379/0',      # Redis as broker
    backend='redis://localhost:6379/0'      # Redis as result backend
)


@app.task(name='tasks.map_total_and_avg_duration')
def map_total_and_avg_duration(songs):
    total_duration = 0

    for song in songs:
        dt = datetime.strptime(song['Length'], "%M:%S")
        song_duration_seconds = dt.minute * 60 + dt.second
        total_duration += song_duration_seconds

    return {
        'len': len(songs),
        'total_duration': total_duration
    }

@app.task(name = "tasks.reduce_total_and_avg_duration")
def reduce_total_and_avg_duration(songs):

    total_duration = 0
    total_songs = 0

    for song in songs:
        total_songs += song['len']
        total_duration += song['total_duration']

    return {
        'total': total_duration,
        'average': total_duration/total_songs
    }

@app.task(name = "tasks.map_explicit_popularity")
def map_explicit_popularity(songs):

    yes_popularity_sum = 0
    numb_of_yes = 0
    no_popularity_sum = 0
    numb_of_no = 0

    for song in songs:

        if song['Explicit'] == 'Yes':
            yes_popularity_sum += int(song['Popularity'])
            numb_of_yes += 1

        elif song['Explicit'] == 'No':
            no_popularity_sum += int(song['Popularity'])
            numb_of_no += 1

    return {
        'yes_popularity_sum':yes_popularity_sum,
        'numb_of_yes': numb_of_yes,
        'no_popularity_sum': no_popularity_sum,
        'numb_of_no': numb_of_no
    }

@app.task(name = "tasks.reduce_explicit_popularity")
def reduce_explicit_popularity(songs):

    total_numb_of_yes = 0
    total_numb_of_no = 0

    total_pop_of_yes = 0
    total_pop_of_no = 0

    for song in songs:
        total_numb_of_yes += song['numb_of_yes']
        total_numb_of_no += song['numb_of_no']
        total_pop_of_yes += song['yes_popularity_sum']
        total_pop_of_no += song['no_popularity_sum']

    return {
        "explicit-popularity": {
            'yes':  total_pop_of_yes / total_numb_of_yes,
            'no': total_pop_of_no / total_numb_of_no
            }
    }


@app.task(name = "tasks.map_artist_popularity")
def map_artist_popularity(songs):

    artist_counts = defaultdict(float)
    artist_popularity = defaultdict(float)

    to_skip = ['feat', 'Feat', ',']

    for song in songs:
        artists = song['Artist(s)']

        include = True

        for skip in to_skip:
            if skip in artists:
                include = False
                break

        if include:
            artist_counts[artists] += 1
            artist_popularity[artists] += float(song['Popularity'])

    return dict(artist_counts), dict(artist_popularity)

@app.task(name = "tasks.reduce_artist_popularity")
def reduce_artist_popularity(stats):

    total_counts = defaultdict(float)
    total_popularity = defaultdict(float)

    for counts, popularity in stats:
        for artist, count in counts.items():
            total_counts[artist] += count
            total_popularity[artist] += popularity.get(artist, 0)

    artist_avg_popularity = {
        artist: (count, total_popularity[artist] / count)  # do not forget it to dividy by count
        for artist, count in total_counts.items()
        if count > 0  
    }

    top_100 = sorted(
        artist_avg_popularity.items(),
        key = lambda item: item[1][0],  # sort by count (item[1][0])
        reverse = True
    )[:100]

    return { 
        "artist-popularity": {
            artist: avg_pop for artist, ( cnt , avg_pop) in top_100
        }
    }

@app.task(name = "tasks.map_danceability")
def map_danceability(songs):

    first_partition = defaultdict(int)
    middle_partition = defaultdict(int)
    last_partition = defaultdict(int)

    first_date = datetime(2002, 1, 1) # IDK is this correct
    second_date = datetime(2013, 1, 1)

    for song in songs:
        if song['Release Date'] is None:
            continue

        if song['Danceability'] is None:
            song['Danceability'] = 0

        release_date = datetime.strptime(song['Release Date'], "%Y-%m-%d")

        if release_date < first_date:
            first_partition['Danceability'] += int(song['Danceability'])
            first_partition['Total'] += 1

        elif release_date < second_date:
            middle_partition['Danceability'] += int(song['Danceability'])
            middle_partition['Total'] += 1

        else:
            last_partition['Danceability'] += int(song['Danceability'])
            last_partition['Total'] += 1

    return dict(first_partition), dict(middle_partition), dict(last_partition)

@app.task(name = "tasks.reduce_danceability")
def reduce_danceability(stats):

    first_danceability = 0
    first_total = 0

    middle_danceability = 0
    middle_total = 0

    third_danceability = 0
    third_total = 0

    for first, middle, third in stats:

        if first.get('Total', 0) > 0:
            first_danceability += first.get('Danceability', 0)
            first_total += first['Total']

        if middle.get('Total', 0) > 0:
            middle_danceability += middle.get('Danceability', 0)
            middle_total += middle['Total']

        if third.get('Total', 0) > 0:
            third_danceability += third.get('Danceability', 0)
            third_total += third['Total']

    return {
        "dancebyyear": {
            "before-2001" : first_danceability / first_total,
            "2001-2012": middle_danceability / middle_total,
            "after-2012" : third_danceability / third_total
        }
    }

@app.task(name = "tasks.combine_final_results")
def combine_final_results(results):
    ans = {}

    for result in results:
        ans.update(result)

    return ans
