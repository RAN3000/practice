import numpy as np
import cv2
import time

########## MAIN FUNCTIONS ###########
#login screens
def login(): #wait then simulate click and go to next screen
    show(login1)
    cv2.waitKey(0)
    clickAt(login1, int(width/2 + width/5), int(height/2 + height/3.5))
    show(login2)
    cv2.waitKey(0)
    clickAt(login2, int(width/2 + width/5.7), int(height/2 + height/3.2))

#settings screen
def settings():
    show(settings1)
    cv2.waitKey(0)

#big-video yo
def video():
    end = False #helper for main while loop
    t0 = time.time() #time at the start of the video
    t0minus = t0 - 336 #default time shown on demo
    t1, timer = 0, 0 #for managing shoot-on-sad

    #params for Lukas-Kanade sparse OF
    #feature_params = dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7)
    #lk_params = dict(winSize=(15,15), maxLevel=2, criteria=(cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_COUNT, 10, 0.03))
    #color = np.random.randint(0,255,(100,3))
    #ret, old_f = cap.read()
    #old_f = crop_middle(old_f)
    #old_gray = cv2.cvtColor(old_f, cv2.COLOR_BGR2GRAY)
    #p0 = cv2.goodFeaturesToTrack(old_gray, mask=None, **feature_params)
    #mask = np.zeros_like(old_f)

    first, congrats = True, False #helpers

    #main while loop
    while(not end):
        #read and minimum elaboration on frame
        ret, frame = cap.read()
        frame = crop_middle(frame)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        #init classifier
        smiles = smile_cc.detectMultiScale(gray, 1.1, 39)

        #time management
        if time.time() - t0 > 5: #give a little break before checking for not smile
            if smiles == (): #check if no smile detected
                if t1 == 0:
                    t1 = time.time() #init timer
                else:
                    timer += time.time()-t1 #update timer
            else: #reset timer on smile on screen
                t1 = 0
                timer = 0

        #Lukas-Kanade calculation
        #p1, st, err = cv2.calcOpticalFlowPyrLK(old_gray, gray, p0, None, **lk_params)

        #if first == True or p1 is not None:
        #    good_new = p1[st==1]
        #    good_old = p0[st==1]
        #    first = False
        #else:
        #    postCongrats()
        #    congrats = True
        #    break

        #for i,(new,old) in enumerate(zip(good_new, good_old)):
        #    a,b = new.ravel()
        #    c,d = old.ravel()
        #    mask = cv2.line(mask, (a,b), (c,d), color[i].tolist(), 2)
        #    frame = cv2.circle(frame, (a,b), 5,color[i].tolist(),-1)
        #frame = cv2.add(frame, mask)

        #draw rectangle around smiles
        for (x,y,w,h) in smiles:
            cv2.rectangle(frame, (x,y), (x+w, y+h), 255, 2)

        frame = drawTime(frame, t0minus)

        show(frame, 'video')

        #check exiting condition
        if timer >= 20:
            end = True

        k = cv2.waitKey(3)
        if k == 27:
            break

        #other Lukas-Kanade lines
        #old_gray = gray.copy()
        #p0 = good_new.reshape(-1,1,2)

    if not congrats: #check if out-of-camera screen in on
        postvideo(t0minus)

#post video frame, frame around picture taken
def postvideo(t):
    ret, frame = cap.read() #takes last frame from video
    frame = crop_middle(frame)
    frame = drawTime(frame, t, post=True)
    show(frame, 'post')

#stats screen
def stats():
    show(stats_screen)
    cv2.waitKey(0)

#post congrats screen
#def postCongrats():
#    show(post_congrats)
#    cv2.waitKey(0)


########### HELPER FUNCTIONS ###########
#helper function to draw time
def drawTime(frame, t0minus, post=False):
    rect_w = 140
    rect_h = 60
    xt = int(width/2 - rect_w/2)
    x = int(width/2)
    yt = int(height - height/6 - rect_h/2.5) if not post else int(height - height/7)
    y = int(height+width*1.6) if not post else int(height+width*1.75)
    rect_window = cv2.circle(frame, (x,y), width*2, (35,88,245), -1)
    t = '\''.join([str(int((time.time()-t0minus)/60)), str(int(time.time()-t0minus)%60)])
    rect_window = cv2.putText(frame, t, (xt+6, yt+rect_h-7), font, 1.8, (255,255,255), 2, cv2.LINE_AA)
    frame = cv2.addWeighted(frame, 0.4, frame, 1, 0)
    return frame

#simulate Android-like clicks on screen
def clickAt(img, x, y):
    empty = np.zeros(img.shape, np.uint8)
    for i in range(3):
        click2 = cv2.circle(empty, (x, y), 8, (255,255,255), -1)
        show(cv2.addWeighted(img, 1,click2, i/3, 0))
        cv2.waitKey(60)

#helper function to show images
def show(img, which=None):
    if which == 'video':
        bg = cv2.bitwise_and(img, img, mask=video_overlay_mask)
        fg = cv2.bitwise_and(video_overlay, video_overlay, mask=video_overlay_inv_mask)
        img = cv2.add(bg, fg)
    if which == 'post':
        bg = cv2.bitwise_and(img, img, mask=post_overlay_mask)
        fg = cv2.bitwise_and(post_overlay, post_overlay, mask=post_overlay_inv_mask)
        img = cv2.add(bg, fg)
        cv2.imshow('Hu-Alert', img)
        cv2.waitKey(0)
        clickAt(img, int(width-width/8), int(height-height/7.5))
    cv2.imshow('Hu-Alert', img)

#helper function to load images
def load(toload):
    return cv2.resize(cv2.imread(toload), (width, height))

#crop images in the middle, predefined width and height
def crop_middle(img):
    middle_x = img.shape[1]/2
    middle_y = img.shape[0]/2
    return img[int(middle_y-height/2):int(middle_y+height/2), int(middle_x-width/2):int(middle_x+width/2)]

########### INIT ###########
#init capture devices
cap = cv2.VideoCapture(0);

#init cascade classifier
smile_cc = cv2.CascadeClassifier('haarcascade_smile.xml')

#rendering parameters
width = 270
height = 480
font = cv2.FONT_HERSHEY_SIMPLEX

#load video overlay, generate various masks for overlaing
video_overlay = load('overlay.png')
video_overlay_hsv = cv2.cvtColor(video_overlay, cv2.COLOR_BGR2HSV)
video_overlay_mask = cv2.inRange(video_overlay_hsv, np.array([-40,28,28]), np.array([40,255,255]))
video_overlay_inv_mask = cv2.bitwise_not(video_overlay_mask)
#load login screen static images
login1 = load('login1.png')
login2 = load('login2.png')
#load settings screen static image
settings1 = load('settings1.png')
#load post-video screen overlay, generate various masks for overlaing
post_overlay = load('post.png')
post_overlay_hsv = cv2.cvtColor(post_overlay, cv2.COLOR_BGR2HSV)
post_overlay_mask = cv2.inRange(post_overlay_hsv, np.array([-40,28,28]), np.array([40,255,255]))
post_overlay_inv_mask = cv2.bitwise_not(post_overlay_mask)
#post_congrats = load('postcongrats.png')
stats_screen = load('stats.png')
#empty jolly image
empty = np.zeros(video_overlay.shape, np.uint8)


#run everything
login()
settings()
video()
stats()

#close everything decently
cv2.destroyAllWindows()
cap.release()
