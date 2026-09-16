import cv2
import numpy as np

# ==========================================
# 1. LOAD IMAGE
# ==========================================

image = cv2.imread("image.jpg")

if image is None:
    print("Error: Could not find image.jpg")
    exit()

# Make a copy for drawing the results
output = image.copy()


# ==========================================
# 2. CONVERT IMAGE TO GRAYSCALE
# ==========================================

gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)


# ==========================================
# 3. BLUR THE IMAGE
# ==========================================

gray = cv2.medianBlur(gray, 5)


# ==========================================
# 4. DETECT CIRCLES USING HOUGH CIRCLES
# ==========================================

circles = cv2.HoughCircles(
    gray,
    cv2.HOUGH_GRADIENT,

    dp=1.2,
    minDist=70,

    param1=100,
    param2=40,

    minRadius=40,
    maxRadius=65
)


# ==========================================
# 5. CHECK IF CIRCLES WERE FOUND
# ==========================================

if circles is None:

    print("No circles detected.")

else:

    # Convert coordinates/radius to integers
    circles = np.round(circles[0, :]).astype("int")

    print("Number of coins detected:", len(circles))
    print()

    # ==========================================
    # 6. SORT CIRCLES
    # ==========================================

    # Sort roughly from top to bottom
    circles = sorted(circles, key=lambda c: (c[1], c[0]))


    # ==========================================
    # 7. DRAW EACH DETECTED COIN
    # ==========================================

    for i, (x, y, r) in enumerate(circles, start=1):

        # Draw circle around coin
        cv2.circle(
            output,
            (x, y),
            r,
            (0, 255, 0),
            3
        )

        # Draw center point
        cv2.circle(
            output,
            (x, y),
            3,
            (0, 0, 255),
            -1
        )

        # Write coin number
        cv2.putText(
            output,
            str(i),
            (x - 10, y + 5),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.6,
            (0, 255, 0),
            2
        )

        # Print information
        print(
            f"Coin {i}: "
            f"Center=({x}, {y}), "
            f"Radius={r}"
        )


# ==========================================
# 8. SAVE OUTPUT
# ==========================================

cv2.imwrite("output.jpg", output)

print()
print("Output saved as output.jpg")


# ==========================================
# 9. DISPLAY RESULT
# ==========================================

cv2.imshow("Detected Coins", output)


# Press Q to quit
while True:

    key = cv2.waitKey(1) & 0xFF

    if key == ord("q"):
        break


# ==========================================
# 10. CLOSE WINDOWS
# ==========================================

cv2.destroyAllWindows()
