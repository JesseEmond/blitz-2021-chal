import argparse
import json
import requests
import time


sess = requests.session()


def send_chal(data):
    """Returns (time_ms, score, sln)."""
    start_time = time.time()
    r = sess.post('http://localhost:27178/microchallenge', data=data)
    end_time = time.time()

    time_s = end_time - start_time
    score = max(0, 3 - time_s)
    return time_s * 1000, score, r.text


def slow_solve(challenge):
    track, items = challenge['track'], challenge['items']
    running_sum = [0] * (len(track) + 1)
    for i in range(len(track)):
        running_sum[i+1] = running_sum[i] + track[i]
    ans = [running_sum[max(start, end)] - running_sum[min(start, end)]
           for start, end in items]
    print(f'Max number len: {max(len(str(n)) for n in ans)}')
    return '['+','.join(str(n) for n in ans)+']'


def benchmark(verify):
    with open('./testcases.txt') as f:
        lines = [line.strip() for line in f.readlines() if 'items' in line]
        lines = [line[2:-1] if line.startswith('b\'') else line for line in lines]
    all_challenges = [json.loads(line.replace('\'', '"')) for line in lines]
    sets = [all_challenges[i:i+75] for i in range(0, len(all_challenges), 75)]

    for set_idx, challenges in enumerate(sets):
        print(f'==== SET #{set_idx+1} ====  ({len(challenges)} challenges)')
        total_time_ms = 0
        total_score = 0
        for idx, challenge in enumerate(challenges):
            MEASUREMENTS = 50
            sample_times, sample_scores = [], []
            chal_data = json.dumps(challenge).replace(' ', '')
            for _ in range(MEASUREMENTS):
                time_ms, score, sln = send_chal(chal_data)
                sample_times.append(time_ms)
                sample_scores.append(score)
            sample_times.sort()
            sample_scores.sort()
            time_ms = sample_times[len(sample_times)//2]
            score = sample_scores[len(sample_scores)//2]
            print(f'  Challenge #{idx+1}: {len(challenge["track"])} track length,  '
                  f'{len(challenge["items"])} queries,   '
                  f'time: {time_ms:.2f}ms,   score: {score:.2f}')
            if verify:
                real_sln = slow_solve(challenge)
                if sln != real_sln:
                    print("!!! WRONG ANSWER !!!")
                    print("      Got: ")
                    print("      ", sln)
                    print("      Expected: ")
                    print("      ", real_sln)
                    return
            total_time_ms += time_ms
            total_score += score
        print('~ SET STATS ~')
        print(f'Average time: {total_time_ms / len(challenges):.2f}ms')
        print(f' Final score: {total_score:.2f}pts')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Benchmarking tool')
    parser.add_argument('--verify', default=False, action='store_true')

    args = parser.parse_args()
    benchmark(verify=args.verify)
