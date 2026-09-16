import cv2
import numpy as np

# ==============================
# 1. LOAD ORIGINAL IMAGE
# ==============================

image = cv2.imread("image.jpg")

if image is None:
    print("Error: Could not find image.jpg")
    exit()

# Keep a copy of the original image
output = image.copy()

# ==============================
# 2. CONVERT TO HSV
# ==============================

hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

# ==============================
# 3. RED COLOR RANGES
# ==============================

lower_red1 = np.array([0, 100, 100])
upper_red1 = np.array([10, 255, 255])

lower_red2 = np.array([170, 100, 100])
upper_red2 = np.array([180, 255, 255])

# ==============================
# 4. CREATE RED MASK
# ==============================

mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
mask2 = cv2.inRange(hsv, lower_red2, upper_red2)

mask = mask1 + mask2

# ==============================
# 5. REMOVE NOISE
# ==============================

kernel = np.ones((5, 5), np.uint8)

mask = cv2.morphologyEx(
    mask,
    cv2.MORPH_OPEN,
    kernel
)

mask = cv2.morphologyEx(
    mask,
    cv2.MORPH_CLOSE,
    kernel
)

# ==============================
# 6. FIND CONTOURS
# ==============================

contours, _ = cv2.findContours(
    mask,
    cv2.RETR_EXTERNAL,
    cv2.CHAIN_APPROX_SIMPLE
)

objects = []

# ==============================
# 7. DETECT OBJECTS
# ==============================

for contour in contours:

    area = cv2.contourArea(contour)

    # Ignore small noise
    if area > 500:

        x, y, w, h = cv2.boundingRect(contour)

        objects.append((x, y, w, h))

        # Find center
        center_x = x + w // 2
        center_y = y + h // 2

        # Draw GREEN bounding box
        cv2.rectangle(
            output,
            (x, y),
            (x + w, y + h),
            (0, 255, 0),
            3
        )

        # Put object number
        cv2.putText(
            output,
            f"Object {len(objects)}",
            (x, y - 10),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.7,
            (0, 255, 0),
            2
        )

        # Print coordinates
        print(
            f"Object {len(objects)}: "
            f"X={x}, Y={y}, "
            f"Width={w}, Height={h}, "
            f"Center=({center_x}, {center_y}), "
            f"Area={area}"
        )

# ==============================
# 8. PRINT TOTAL
# ==============================

print()
print("==============================")
print("Number of objects:", len(objects))
print("==============================")

# ==============================
# 9. SAVE OUTPUT IMAGE
# ==============================

cv2.imwrite("output.jpg", output)

print("Output saved as output.jpg")

# ==============================
# 10. SHOW ORIGINAL + BOXES
# ==============================

cv2.imshow("Detected Red Objects", output)

# Press Q to close
while True:

    key = cv2.waitKey(1) & 0xFF

    if key == ord('q'):
        break

cv2.destroyAllWindows()
