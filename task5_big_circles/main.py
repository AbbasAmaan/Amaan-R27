import cv2
import numpy as np

# ==========================================
# 1. LOAD IMAGE
# ==========================================

image = cv2.imread("image.jpg")

if image is None:
    print("Error: Could not find image.jpg")
    exit()

# Copy original image
output = image.copy()


# ==========================================
# 2. CONVERT TO GRAYSCALE
# ==========================================

gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)


# ==========================================
# 3. BLUR IMAGE
# ==========================================

gray = cv2.medianBlur(gray, 5)


# ==========================================
# 4. DETECT CIRCLES
# ==========================================

circles = cv2.HoughCircles(
    gray,
    cv2.HOUGH_GRADIENT,

    dp=1.2,

    # Minimum distance between circle centers
    minDist=60,

    # Edge detection settings
    param1=100,

    # Higher = fewer false detections
    param2=45,

    # Circle size range
    minRadius=35,
    maxRadius=100
)


# ==========================================
# 5. CHECK IF CIRCLES WERE FOUND
# ==========================================

if circles is None:

    print("No circles detected.")

else:

    # Convert to integers
    circles = np.round(circles[0, :]).astype("int")

    # ==========================================
    # 6. FILTER CIRCLES
    # ==========================================

    big_circles = []

    for x, y, r in circles:

        # Keep only circles with radius >= 40
        if r >= 40:

            big_circles.append((x, y, r))


    # ==========================================
    # 7. SORT CIRCLES
    # ==========================================

    big_circles = sorted(
        big_circles,
        key=lambda c: (c[1], c[0])
    )


    # ==========================================
    # 8. DRAW CIRCLES
    # ==========================================

    for i, (x, y, r) in enumerate(big_circles, start=1):

        # Draw green circle
        cv2.circle(
            output,
            (x, y),
            r,
            (0, 255, 0),
            3
        )

        # Draw red center point
        cv2.circle(
            output,
            (x, y),
            3,
            (0, 0, 255),
            -1
        )

        # Write number
        cv2.putText(
            output,
            str(i),
            (x - 10, y + 5),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2
        )

        # Print information
        print(
            f"Circle {i}: "
            f"Center=({x}, {y}), "
            f"Radius={r}"
        )


    # ==========================================
    # 9. PRINT TOTAL
    # ==========================================

    print()
    print("==============================")
    print("Big circles detected:", len(big_circles))
    print("==============================")


# ==========================================
# 10. SAVE OUTPUT
# ==========================================

cv2.imwrite("output.jpg", output)

print("Output saved as output.jpg")


# ==========================================
# 11. DISPLAY OUTPUT
# ==========================================

cv2.imshow("Detected Big Circles", output)


# ==========================================
# 12. PRESS Q TO EXIT
# ==========================================

while True:

    key = cv2.waitKey(1) & 0xFF

    if key == ord("q"):
        break


# ==========================================
# 13. CLOSE WINDOWS
# ==========================================

cv2.destroyAllWindows()
